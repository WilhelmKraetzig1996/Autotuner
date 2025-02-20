#pragma once

#include <kiss_fftr.h>

#include "autotuner/AudioBuffer.h"
#include "autotuner/INormalizedSquareDifferenceCalculator.h"


namespace autotuner::backend
{
    class NormalizedSquareDifferenceCalculator : public INormalizedSquareDifferenceCalculator
    {
    public:
        NormalizedSquareDifferenceCalculator(size_t signalSize, size_t detectionWindowSize);
        ~NormalizedSquareDifferenceCalculator() override;

        const AudioBuffer process(const CircularAudioBuffer& signal) override;

        const AudioBuffer& akf();

    private:
        size_t signalSize_;
        size_t detectionWindowSize_;
        AudioBuffer nsdf_;
        AudioBuffer signalPadded_;
        AudioBuffer signalSquared_;
        AudioBuffer autoCorrelationFunction_;

        kiss_fftr_cfg fftSetup_;
        kiss_fftr_cfg ifftSetup_;
        kiss_fft_cpx* complexBuffer_;
    };
}
