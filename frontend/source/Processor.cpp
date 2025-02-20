#include "Processor.h"
#include "autotuner/Backend.h"
#include "autotuner/GoalPeriodCalculator.h"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace autotuner
{
    namespace
    {

        juce::StringArray KEY_SELECTIONS = {"A", "Bb", "B", "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab"};

        constexpr backend::GoalPeriodCalculator::Key DEFAULT_KEY = backend::GoalPeriodCalculator::Key::C_MAJOR;
        constexpr float MIN_PROBABILITY = 0.9f;
        constexpr float REL_MAX_DIFFERENCE = 0.01f;

        std::map<std::string, backend::GoalPeriodCalculator::Key> KEY_MAP = {
            {KEY_SELECTIONS[0].toStdString(), backend::GoalPeriodCalculator::Key::A_MAJOR},
            {KEY_SELECTIONS[1].toStdString(), backend::GoalPeriodCalculator::Key::B_FLAT_MAJOR},
            {KEY_SELECTIONS[2].toStdString(), backend::GoalPeriodCalculator::Key::B_MAJOR},
            {KEY_SELECTIONS[3].toStdString(), backend::GoalPeriodCalculator::Key::C_MAJOR},
            {KEY_SELECTIONS[4].toStdString(), backend::GoalPeriodCalculator::Key::C_SHARP_MAJOR},
            {KEY_SELECTIONS[5].toStdString(), backend::GoalPeriodCalculator::Key::D_MAJOR},
            {KEY_SELECTIONS[6].toStdString(), backend::GoalPeriodCalculator::Key::E_FLAT_MAJOR},
            {KEY_SELECTIONS[7].toStdString(), backend::GoalPeriodCalculator::Key::E_MAJOR},
            {KEY_SELECTIONS[8].toStdString(), backend::GoalPeriodCalculator::Key::F_MAJOR},
            {KEY_SELECTIONS[9].toStdString(), backend::GoalPeriodCalculator::Key::F_SHARP_MAJOR},
            {KEY_SELECTIONS[10].toStdString(), backend::GoalPeriodCalculator::Key::G_MAJOR},
            {KEY_SELECTIONS[11].toStdString(), backend::GoalPeriodCalculator::Key::A_FLAT_MAJOR}};
    }

    Processor::Processor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Output", juce::AudioChannelSet::stereo()).withInput("Sidechain", juce::AudioChannelSet::stereo()))
    , parameters_(
          *this, nullptr, juce::Identifier("Autotuner"),
          {std::make_unique<juce::AudioParameterChoice>("key", "Key (Major)", KEY_SELECTIONS, static_cast<int>(DEFAULT_KEY)),
           std::make_unique<juce::AudioParameterFloat>("minProbability", "Minimum Period detection Probability", 0.0f, 1.0f, MIN_PROBABILITY)})
    , lastKey_(DEFAULT_KEY)
    , lastMinProbability_(0.0f)
    , engine_(std::make_unique<backend::Backend>(getBlockSize(), getSampleRate(), DEFAULT_KEY))
    {}

    bool Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
    {
        return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet() && !layouts.getMainInputChannelSet().isDisabled();
    }

    void Processor::prepareToPlay(double, int)
    {
        backend::GoalPeriodCalculator::Key currKey = static_cast<backend::GoalPeriodCalculator::Key>(static_cast<int>(*parameters_.getRawParameterValue("key")));
        engine_ = std::make_unique<backend::Backend>(getBlockSize(), getSampleRate(), currKey);
        float currMinProbability = static_cast<float>(*parameters_.getRawParameterValue("minProbability"));
        engine_->setMinimumPeriodProbability(currMinProbability);
        std::cout << "prepareToPlay called " << currMinProbability << std::endl;
    }

    void Processor::releaseResources() {}

    void Processor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
    {

        const auto channelBuffers = const_cast<float**>(buffer.getArrayOfWritePointers());
        const auto numSamples = static_cast<size_t>(buffer.getNumSamples());
        backend::GoalPeriodCalculator::Key currKey = static_cast<backend::GoalPeriodCalculator::Key>(static_cast<int>(*parameters_.getRawParameterValue("key")));
        if (currKey != lastKey_)
        {
            engine_->setKey(currKey);
            lastKey_ = currKey;
        }
        float currMinProbability = static_cast<float>(*parameters_.getRawParameterValue("minProbability"));
        if (std::fabs(currMinProbability - lastMinProbability_) > currMinProbability * REL_MAX_DIFFERENCE)
        {
            engine_->setMinimumPeriodProbability(currMinProbability);
            lastMinProbability_ = currMinProbability;
        }

        backend::AudioBuffer inBuffer(channelBuffers[0], numSamples);
        backend::AudioBuffer outBuffer(numSamples);
        engine_->process(inBuffer, outBuffer);
        for (size_t iChannel = 0; iChannel < static_cast<size_t>(buffer.getNumChannels()); iChannel++)
            for (size_t iSample = 0; iSample < numSamples; iSample++)
                channelBuffers[iChannel][iSample] = outBuffer[iSample];
    }

    juce::AudioProcessorEditor* Processor::createEditor() { return new juce::GenericAudioProcessorEditor(*this); }
    bool Processor::hasEditor() const { return true; }
    const juce::String Processor::getName() const { return "Autotuner"; }
    bool Processor::acceptsMidi() const { return false; }
    bool Processor::producesMidi() const { return false; }
    double Processor::getTailLengthSeconds() const { return 0.0; }
    int Processor::getNumPrograms() { return 1; }
    int Processor::getCurrentProgram() { return 0; }
    void Processor::setCurrentProgram(int) {}
    const juce::String Processor::getProgramName(int) { return {}; }
    void Processor::changeProgramName(int, const juce::String&) {}

    bool Processor::isVST2() const noexcept { return (wrapperType == wrapperType_VST); }

    void Processor::getStateInformation(juce::MemoryBlock& destData)
    {
        auto state = parameters_.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void Processor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName(parameters_.state.getType()))
                parameters_.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}
