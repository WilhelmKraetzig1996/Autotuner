#include <cassert>

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/NormalizedSquareDifferenceCalculator.h"
#include "autotuner/PeriodDetector.h"


namespace autotuner::backend
{
    namespace
    {
        constexpr float MIN_PROBABILITY = 0.9f;
        constexpr size_t MAX_RECURSION_DEPTH = 20;
        constexpr float RELATIVE_THESHOLD_STEPSIZE = 0.05f;
    }


    PeriodDetector::PeriodDetector(size_t signalSize, size_t periodDetectionLength)
    : nsdfCalculator_(std::make_unique<NormalizedSquareDifferenceCalculator>(signalSize, periodDetectionLength))
    , minProbability_(MIN_PROBABILITY)
    {}


    float PeriodDetector::process(const CircularAudioBuffer& signal)
    {
        return detectPeriod_(detectPeriods_(nsdfCalculator_->process(signal), 0));
    }

    float PeriodDetector::detectPeriod_(std::vector<PossiblePeriod> periods)
    {
        auto probabilitySum = 0.0f;
        auto probabilityCounter = 0ul;
        for (PossiblePeriod period: periods)
        {
            if (period.probability > minProbability_)
            {
                probabilityCounter++;
                probabilitySum += period.period;
            }
        }
        if (probabilityCounter == 0 || periods[0].probability < minProbability_)
            return 0.0f;
        return probabilitySum / static_cast<float>(probabilityCounter);
    }

    std::vector<PossiblePeriod> PeriodDetector::detectPeriods_(AudioBuffer nsdf, size_t recursionDepth)
    {
        auto peaks = getAllPeaks_(nsdf);
        auto probablePeaks = getAllFirstPeaksAboveThresholds_(peaks, nsdf.max());

        if (probablePeaks.size() == 0)
            return std::vector<PossiblePeriod> {PossiblePeriod {0.0f, 0.0f}};

        auto periodIndex = getFirstSignificantPeak_(probablePeaks);

        std::vector<PossiblePeriod> periods;
        periods.push_back(PossiblePeriod({static_cast<float>(periodIndex), nsdf[periodIndex]}));
        if (nsdf.size() >= periodIndex * 3 && recursionDepth < MAX_RECURSION_DEPTH)
        {
            auto subNsdf = nsdf[{periodIndex, nsdf.size()}];
            auto subPeriods = detectPeriods_(subNsdf, recursionDepth + 1);
            periods.insert(periods.end(), subPeriods.begin(), subPeriods.end());
        }

        return periods;
    }

    std::vector<Peak> PeriodDetector::getAllPeaks_(const AudioBuffer& nsdf)
    {
        std::vector<Peak> peaks;
        for (size_t i = 1; i < nsdf.size() - 1; i++)
            if (nsdf[i] >= minProbability_ && nsdf[i - 1] <= nsdf[i] && nsdf[i + 1] <= nsdf[i])
                peaks.push_back({i, nsdf[i]});
        return peaks;
    }

    std::vector<Peak> PeriodDetector::getAllFirstPeaksAboveThresholds_(const std::vector<Peak>& peaks, float globalMaximum)
    {
        std::vector<Peak> probablePeaks;
        auto threshold = 0.0f;
        for (float relativeThreshold = 0.0f; relativeThreshold <= 1.0f; relativeThreshold += RELATIVE_THESHOLD_STEPSIZE)
        {
            threshold = globalMaximum * relativeThreshold;
            for (Peak peak: peaks)
                if (peak.value > threshold)
                {
                    probablePeaks.push_back(peak);
                    break;
                }
        }
        return probablePeaks;
    }

    size_t PeriodDetector::getFirstSignificantPeak_(std::vector<Peak> peaks)
    {
        std::vector<PeakCount> peakCounts;
        auto peakFound = false;
        for (Peak peak: peaks)
        {
            peakFound = false;
            for (PeakCount peakCount: peakCounts)
                if (peak.idx == peakCount.idx)
                {
                    peakCount.count++;
                    peakFound = true;
                    break;
                }
            if (!peakFound)
                peakCounts.push_back({peak.idx, 1});
        }

        auto highestPeakCount = PeakCount {0, 0};
        for (PeakCount peakCount: peakCounts)
            if (peakCount.count > highestPeakCount.count)
                highestPeakCount = peakCount;

        return highestPeakCount.idx;
    }
    void PeriodDetector::setMinimumPeriodProbability(float probability)
    {
        assert(probability >= 0);
        assert(probability <= 1);
        minProbability_ = probability;
    }
}
