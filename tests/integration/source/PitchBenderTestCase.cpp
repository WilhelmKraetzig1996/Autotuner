#include "gtest/gtest.h"
#include <fstream>
#include <functional>
#include <gtest/gtest.h>
#include <ios>
#include <sstream>

#include "autotuner/AudioBuffer.h"
#include "autotuner/Backend.h"
#include "autotuner/GoalPeriodCalculator.h"


namespace autotuner::backend
{
    using namespace testing;
    class IntegrationTest_PitchBender : public TestWithParam<std::tuple<size_t, size_t>>
    {
    protected:
        void SetUp() override
        {
            blockSize_ = std::get<0>(GetParam());
            samplerate_ = std::get<1>(GetParam());
        }

        AudioBuffer readFloatBinFile_(std::string filename)
        {
            std::ifstream file(filename, std::ios::binary);

            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            AudioBuffer audio(static_cast<size_t>(size) / sizeof(float));
            file.read(reinterpret_cast<char*>(&audio[0]), size);
            return audio;
        }

        void writeFloatBinFile_(std::string filename, AudioBuffer& audio)
        {
            std::ofstream file(filename, std::ios::binary);
            file.write(reinterpret_cast<char*>(&audio[0]), static_cast<std::streamsize>(audio.size() * sizeof(float)));
            file.close();
        }

        size_t blockSize_;
        size_t samplerate_;
    };

    TEST_P(IntegrationTest_PitchBender, instantiate)
    {
        Backend(blockSize_, samplerate_, GoalPeriodCalculator::Key::F_MAJOR);
    }

    TEST_P(IntegrationTest_PitchBender, process)
    {
        std::stringstream inputFile;
        inputFile << "./tests/resources/in_" << samplerate_ << ".bin";
        AudioBuffer input = readFloatBinFile_(inputFile.str());
        std::stringstream refFile;
        refFile << "./tests/resources/ref_" << blockSize_ << "_" << samplerate_ << ".bin";
        AudioBuffer ref = readFloatBinFile_(refFile.str());
        AudioBuffer output(input.size());
        Backend bender(blockSize_, samplerate_, GoalPeriodCalculator::Key::E_MAJOR);

        for (size_t i = 0; i < input.size() - blockSize_; i += blockSize_)
        {
            AudioBuffer currOutput = output[{i, i + blockSize_ - 1}];
            bender.process(input[{i, i + blockSize_ - 1}], currOutput);
        }
        for (size_t i = 0; i < input.size(); i++)
            EXPECT_NEAR(output[i], ref[i], 7e-5);
    }

    INSTANTIATE_TEST_SUITE_P(IntegrationTest_PitchBender, IntegrationTest_PitchBender,
                             Combine(Values(64, 128, 256, 512, 1024), Values(44100, 48000, 96000)));
}
