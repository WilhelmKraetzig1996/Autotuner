#pragma once

#include <memory>

#include "IPitchBender.h"
#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/IValidPeriodCalculator.h"


namespace autotuner::backend
{
    class PitchBender : public IPitchBender
    {
    public:
        PitchBender(size_t blockSize, size_t inputBufferSize, std::shared_ptr<IValidPeriodCalculator> periodCalculator);

        void process(const AudioBuffer& input, AudioBuffer& output) override;

    private:
        void updateWindow_(size_t period);

        size_t calculateNumOutputPeriods_(size_t blockSize, float goalPeriod);
        void wrapInputStartToValidBoundaries_(size_t inputPeriod, size_t inputBufferNeeded);

        CircularAudioBuffer inputBuffer_;
        CircularAudioBuffer outputBuffer_;
        std::shared_ptr<IValidPeriodCalculator> periodCalculator_;
        int inputStart_;
        int outputStart_;
        AudioBuffer window_;
    };
}
