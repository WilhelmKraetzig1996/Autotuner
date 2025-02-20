#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <gtest/gtest.h>

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/PeriodDetector.h"


namespace autotuner::backend
{
    using namespace testing;

    namespace
    {
        float SAMPLERATE = 48000.0f;

        float getWhiteNoiseSample(float level)
        {
            return ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2 - 1) * level;
        }
    }

    class IntegrationTest_PeriodDetector : public TestWithParam<std::tuple<float, float, size_t>>
    {
    protected:
        void SetUp() override
        {
            auto params = GetParam();
            frequency_ = std::get<0>(params);
            noiseLevel_ = std::get<1>(params);
            size_ = std::get<2>(params);
        }


        float frequency_;
        float noiseLevel_;
        size_t size_;
    };

    TEST_P(IntegrationTest_PeriodDetector, detectPeriod)
    {
        PeriodDetector detector = PeriodDetector(size_, size_);
        float pi = 2 * std::acos(0.0f);
        AudioBuffer signal(size_);
        CircularAudioBuffer circSignal(size_);

        for (size_t i_sample = 0; i_sample < size_; i_sample++)
            signal[i_sample] = std::sin(2 * pi * frequency_ * static_cast<float>(i_sample) / SAMPLERATE) + getWhiteNoiseSample(noiseLevel_);

        circSignal.push(signal);

        float detectedFrequency = SAMPLERATE / detector.process(circSignal);

        ASSERT_NEAR(detectedFrequency, frequency_, frequency_ * 0.005f);
    }

    INSTANTIATE_TEST_SUITE_P(IntegrationTest_PeriodDetector, IntegrationTest_PeriodDetector,
                             Combine(
                                 Values(200.0f, 810.5f, 4384.0f),
                                 Values(0.0f, 0.01f, 0.1f),
                                 Values(1000, 3000, 10000)));
}
