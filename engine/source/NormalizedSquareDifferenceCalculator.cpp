
#include "autotuner/NormalizedSquareDifferenceCalculator.h"


namespace autotuner::backend
{
    NormalizedSquareDifferenceCalculator::NormalizedSquareDifferenceCalculator(size_t signalSize, size_t detectionWindowSize)
    : signalSize_(signalSize)
    , detectionWindowSize_(detectionWindowSize)
    , nsdf_(signalSize)
    , signalPadded_(signalSize * 3)
    , signalSquared_(signalSize)
    , autoCorrelationFunction_(signalSize * 2)
    , fftSetup_(kiss_fftr_alloc(static_cast<int>(signalSize * 2), false, nullptr, nullptr))
    , ifftSetup_(kiss_fftr_alloc(static_cast<int>(signalSize * 2), true, nullptr, nullptr))
    , complexBuffer_(new kiss_fft_cpx[signalSize * 2])
    {
        for (size_t i = 0; i < signalSize; ++i)
        {
            complexBuffer_[i].r = 0.0f;
            complexBuffer_[i].i = 0.0f;
        }
    }

    NormalizedSquareDifferenceCalculator::~NormalizedSquareDifferenceCalculator()
    {
        if (fftSetup_ != nullptr)
            kiss_fftr_free(fftSetup_);
        if (ifftSetup_ != nullptr)
            kiss_fftr_free(ifftSetup_);
        delete[] complexBuffer_;
    }

    const AudioBuffer NormalizedSquareDifferenceCalculator::process(const CircularAudioBuffer& signal)
    {
        auto offset = signalSize_ - detectionWindowSize_;
        for (size_t i = 0; i < signalSize_ * 2; i++)
            signalPadded_[i] = 0.0f;

        signal.toAudioBuffer(signalPadded_);

        kiss_fftr(fftSetup_, &signalPadded_[offset], complexBuffer_);

        for (size_t i = 0; i < signalSize_ + 1; i++)
        {
            complexBuffer_[i].r = complexBuffer_[i].r * complexBuffer_[i].r + complexBuffer_[i].i * complexBuffer_[i].i;
            complexBuffer_[i].i = 0;
        }

        kiss_fftri(ifftSetup_, complexBuffer_, &autoCorrelationFunction_[0]);

        for (size_t i = 0; i < signalSize_ * 2; i++)
            autoCorrelationFunction_[i] /= static_cast<float>(signalSize_) * 2;

        for (size_t i = 0; i < signalSize_ - offset; i++)
            signalSquared_[i] = signalPadded_[i + offset] * signalPadded_[i + offset];

        auto squareDifference = 0.0f;
        for (size_t i_signalSubset = 0; i_signalSubset < signalSize_ - 1; i_signalSubset++)
            squareDifference += signalSquared_[1 + i_signalSubset] + signalSquared_[i_signalSubset];

        nsdf_[0] = 2 * autoCorrelationFunction_[1] / squareDifference;

        for (size_t lag = 2; lag < signal.size() - offset; lag++)
        {
            squareDifference -= signalSquared_[lag - 1] + signalSquared_[signalSize_ - offset - lag];
            nsdf_[lag - 1] = 2 * autoCorrelationFunction_[lag] / squareDifference;
        }
        return nsdf_[{0, signalSize_ - 1}];
    }

    const AudioBuffer& NormalizedSquareDifferenceCalculator::akf()
    {
        return autoCorrelationFunction_;
    }
}
