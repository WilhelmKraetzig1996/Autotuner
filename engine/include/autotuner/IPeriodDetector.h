#pragma once

#include "autotuner/CircularAudioBuffer.h"


namespace autotuner::backend
{
    class IPeriodDetector
    {
    public:
        virtual ~IPeriodDetector() = default;
        virtual float process(const CircularAudioBuffer& signal) = 0;
        virtual void setMinimumPeriodProbability(float probability) = 0;
    };
}
