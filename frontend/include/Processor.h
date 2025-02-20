#pragma once

#include "autotuner/Backend.h"
#include "autotuner/GoalPeriodCalculator.h"
#include "autotuner/PitchBender.h"
#include <JuceHeader.h>
#include <memory>

namespace autotuner
{
    class Processor : public juce::AudioProcessor
    {
    public:
        Processor();

        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
        void prepareToPlay(double, int) override;
        void releaseResources() override;
        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;
        const juce::String getName() const override;
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        double getTailLengthSeconds() const override;
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int) override;
        const juce::String getProgramName(int) override;
        void changeProgramName(int, const juce::String&) override;
        bool isVST2() const noexcept;
        void getStateInformation(juce::MemoryBlock& destData) override;
        void setStateInformation(const void* data, int sizeInBytes) override;

    private:
        juce::AudioProcessorValueTreeState parameters_;
        backend::GoalPeriodCalculator::Key lastKey_;
        float lastMinProbability_;
        std::unique_ptr<backend::Backend> engine_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
    };
}
