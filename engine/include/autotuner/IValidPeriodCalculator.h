#pragma once

#include <tuple>

#include "autotuner/CircularAudioBuffer.h"


namespace autotuner::backend
{
    class IValidPeriodCalculator
    {
    public:
        virtual ~IValidPeriodCalculator() = default;
        virtual std::tuple<float, size_t> calculatePeriods(const CircularAudioBuffer& inputBuffer, size_t blockSize) = 0;
        virtual size_t calculateNextPeriod(float goalPeriod) = 0;
        virtual float maxPeriod() = 0;
    };

}
