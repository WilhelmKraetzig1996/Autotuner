#include <cstddef>
#include <gtest/gtest.h>
#include <stdexcept>

#include "autotuner/AudioBuffer.h"


namespace autotuner::backend
{
    using namespace testing;

    class IntegrationTest_AudioBuffer : public TestWithParam<size_t>
    {
    protected:
        void SetUp() override
        {
            bufferSize_ = GetParam();
        }

        size_t bufferSize_;
    };

    TEST_P(IntegrationTest_AudioBuffer, instantiate)
    {
        AudioBuffer audioBuffer = AudioBuffer(bufferSize_);
        for (size_t i = 0; i < bufferSize_; i++)
            ASSERT_EQ(audioBuffer[i], 0.0f);
        ASSERT_EQ(audioBuffer.size(), bufferSize_);
    }

    TEST_P(IntegrationTest_AudioBuffer, operatorIdx)
    {
        AudioBuffer audioBuffer = AudioBuffer(bufferSize_);
        audioBuffer[1] = 1.0f;
        audioBuffer[2] = 3.0f;
        audioBuffer[bufferSize_ - 1] = -1.0f;

        ASSERT_EQ(audioBuffer[1], 1.0f);
        ASSERT_EQ(audioBuffer[2], 3.0f);
        ASSERT_EQ(audioBuffer[bufferSize_ - 1], -1.0f);
    }

    TEST_P(IntegrationTest_AudioBuffer, operatorMultiIdx)
    {
        AudioBuffer audioBuffer = AudioBuffer(bufferSize_);
        AudioBuffer subset = audioBuffer[{1, 3}];
        ASSERT_EQ(subset.size(), 3);
        subset[0] = 1.0f;
        subset[2] = 2.0f;
        ASSERT_EQ(audioBuffer[1], 1.0f);
        ASSERT_EQ(audioBuffer[3], 2.0f);
    }

    TEST_P(IntegrationTest_AudioBuffer, operatorPlusEq)
    {
        AudioBuffer firstBuffer = AudioBuffer(bufferSize_);
        AudioBuffer secondBuffer = AudioBuffer(bufferSize_);
        for (size_t i = 0; i < bufferSize_; i++)
            firstBuffer[i] = 1.0f;
        firstBuffer += secondBuffer;
        for (size_t i = 0; i < bufferSize_; i++)
            ASSERT_EQ(firstBuffer[i], 1.0f);
    }

    TEST_P(IntegrationTest_AudioBuffer, operatorTimesEq)
    {
        AudioBuffer firstBuffer = AudioBuffer(bufferSize_);
        AudioBuffer secondBuffer = AudioBuffer(bufferSize_);
        for (size_t i = 0; i < bufferSize_; i++)
        {
            firstBuffer[i] = 2.0f;
            secondBuffer[i] = 3.0f;
        }
        firstBuffer *= secondBuffer;
        for (size_t i = 0; i < bufferSize_; i++)
            ASSERT_EQ(firstBuffer[i], 6.0f);
    }

    TEST_P(IntegrationTest_AudioBuffer, max)
    {
        AudioBuffer buffer = AudioBuffer(bufferSize_);
        buffer[0] = 1.0f;
        buffer[1] = 5.0f;
        buffer[bufferSize_ - 2] = 6.0f;
        buffer[bufferSize_ - 1] = -7.0f;
        ASSERT_EQ(buffer.max(), 6.0f);
    }

    TEST_P(IntegrationTest_AudioBuffer, copyFrom)
    {
        AudioBuffer firstBuffer = AudioBuffer(bufferSize_);
        AudioBuffer secondBuffer = AudioBuffer(bufferSize_);
        for (size_t i = 0; i < bufferSize_; i++)
        {
            firstBuffer[i] = 2.0f;
            secondBuffer[i] = 3.0f;
        }
        firstBuffer.copyFrom(secondBuffer);
        for (size_t i = 0; i < bufferSize_; i++)
        {
            ASSERT_EQ(firstBuffer[i], 3.0f);
        }
    }

    INSTANTIATE_TEST_SUITE_P(IntegrationTest_AudioBuffer, IntegrationTest_AudioBuffer,
                             Values(5, 10));
}
