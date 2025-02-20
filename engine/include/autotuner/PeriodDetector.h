#pragma once

#include <memory>
#include <vector>

#include "IPeriodDetector.h"
#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/NormalizedSquareDifferenceCalculator.h"


namespace autotuner::backend
{

    namespace
    {
        struct PossiblePeriod
        {
            float period, probability;
        };

        struct Peak
        {
            size_t idx;
            float value;
        };

        struct PeakCount
        {
            size_t idx, count;
        };
    }

    class PeriodDetector : public IPeriodDetector
    {
    public:
        PeriodDetector(size_t signalSize, size_t periodDetectionLength);
        ~PeriodDetector() override = default;

        float process(const CircularAudioBuffer& signal) override;

        void setMinimumPeriodProbability(float probability) override;

    private:
        float detectPeriod_(std::vector<PossiblePeriod> periods);

        std::vector<PossiblePeriod> detectPeriods_(AudioBuffer nsdf, size_t recursionDepth);

        std::vector<Peak> getAllPeaks_(const AudioBuffer& nsdf);
        std::vector<Peak> getAllFirstPeaksAboveThresholds_(const std::vector<Peak>& peaks, float globalMaximum);

        size_t getFirstSignificantPeak_(std::vector<Peak> peaks);

        std::unique_ptr<NormalizedSquareDifferenceCalculator> nsdfCalculator_;

        float minProbability_;
    };
}
