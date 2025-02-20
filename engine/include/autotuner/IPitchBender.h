#pragma once

#include "AudioBuffer.h"


namespace autotuner::backend
{
    class IPitchBender
    {
    public:
        virtual ~IPitchBender() = default;
        virtual void process(const AudioBuffer& input, AudioBuffer& output) = 0;
    };
}
