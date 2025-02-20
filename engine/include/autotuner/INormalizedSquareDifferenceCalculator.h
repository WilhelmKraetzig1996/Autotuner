#pragma once

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"


namespace autotuner::backend
{
    class INormalizedSquareDifferenceCalculator
    {
    public:
        virtual ~INormalizedSquareDifferenceCalculator() = default;
        virtual const AudioBuffer process(const CircularAudioBuffer& signal) = 0;
    };
}
