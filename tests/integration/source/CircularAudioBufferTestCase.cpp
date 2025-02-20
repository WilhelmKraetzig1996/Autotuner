#include <cstddef>
#include <gtest/gtest.h>

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"


namespace autotuner::backend
{
    using namespace testing;

    class IntegrationTest_CircularAudioBuffer : public TestWithParam<std::tuple<size_t, size_t>>
    {
    protected:
        void SetUp() override
        {
            bufferSize_ = std::get<0>(GetParam());
            extraSize_ = std::get<1>(GetParam());
        }

        size_t bufferSize_;
        size_t extraSize_;
    };

    TEST_P(IntegrationTest_CircularAudioBuffer, pushAndRead)
    {
        CircularAudioBuffer circularBuffer(bufferSize_);
        AudioBuffer inputBuffer(bufferSize_);
        AudioBuffer extraBuffer(extraSize_);

        for (size_t i = 0; i < bufferSize_; ++i)
            inputBuffer[i] = static_cast<float>(i);

        for (size_t i = 0; i < extraSize_; ++i)
            extraBuffer[i] = static_cast<float>(i + bufferSize_);

        circularBuffer.push(inputBuffer);
        circularBuffer.push(extraBuffer);

        for (size_t i = 0; i < bufferSize_; ++i)
            EXPECT_EQ(circularBuffer[i], static_cast<float>(i + extraSize_));
    }

    TEST_P(IntegrationTest_CircularAudioBuffer, write)
    {
        CircularAudioBuffer circularBuffer(bufferSize_);

        for (size_t i = 0; i < bufferSize_; ++i)
            circularBuffer[i] = static_cast<float>(i);

        for (size_t i = 0; i < bufferSize_; ++i)
            ASSERT_EQ(circularBuffer[i], static_cast<float>(i));
    }

    TEST_P(IntegrationTest_CircularAudioBuffer, toAudioBuffer)
    {
        CircularAudioBuffer circularBuffer(bufferSize_);
        AudioBuffer inputBuffer(bufferSize_);
        AudioBuffer extraBuffer(extraSize_);

        for (size_t i = 0; i < bufferSize_; ++i)
            inputBuffer[i] = static_cast<float>(i);

        for (size_t i = 0; i < extraSize_; ++i)
            extraBuffer[i] = static_cast<float>(i + bufferSize_);

        circularBuffer.push(inputBuffer);
        circularBuffer.push(extraBuffer);

        AudioBuffer output(bufferSize_);

        circularBuffer.toAudioBuffer(output);

        for (size_t i = 0; i < bufferSize_; ++i)
            EXPECT_EQ(output[i], static_cast<float>(i + extraSize_));
    }

    INSTANTIATE_TEST_SUITE_P(IntegrationTest_CircularAudioBuffer, IntegrationTest_CircularAudioBuffer,
                             Combine(Values(8, 16, 32, 64), Values(1, 2, 3, 4)));
}
