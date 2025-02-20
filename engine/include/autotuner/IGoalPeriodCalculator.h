#pragma once


namespace autotuner::backend
{
    class IGoalPeriodCalculator
    {
    public:
        virtual ~IGoalPeriodCalculator() = default;
        virtual float calculate(float inputPeriod) = 0;
    };
}
