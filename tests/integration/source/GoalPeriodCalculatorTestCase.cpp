#include <array>

#include <gtest/gtest.h>

#include "autotuner/GoalPeriodCalculator.h"


namespace autotuner::backend
{
    using namespace testing;
    class IntegrationTest_GoalPeriodCalculator : public TestWithParam<std::tuple<size_t, std::array<float, 2>, GoalPeriodCalculator::Key>>
    {
    protected:
        void SetUp() override
        {
            auto params = GetParam();
            samplerate_ = std::get<0>(params);
            frequency_ = std::get<1>(params)[0];
            goalFrequency_ = std::get<1>(params)[1];
            key_ = std::get<2>(params);
        }

        size_t samplerate_;
        float frequency_;
        float goalFrequency_;
        GoalPeriodCalculator::Key key_;
    };

    TEST_P(IntegrationTest_GoalPeriodCalculator, calculate)
    {
        GoalPeriodCalculator calculator(samplerate_, key_);
        auto frequency = static_cast<float>(samplerate_) / calculator.calculate(static_cast<float>(samplerate_) / frequency_);
        EXPECT_EQ(frequency, goalFrequency_);
    }

    INSTANTIATE_TEST_SUITE_P(IntegrationTest_GoalPeriodCalculator, IntegrationTest_GoalPeriodCalculator,
                             Combine(Values(44100, 48000, 96000),
                                     Values(std::array<float, 2>({442.f, 440.f}), std::array<float, 2>({218.f, 220.f}), std::array<float, 2>({110.f, 110.f})),
                                     Values(GoalPeriodCalculator::Key::C_MAJOR, GoalPeriodCalculator::Key::A_MAJOR)));

}
