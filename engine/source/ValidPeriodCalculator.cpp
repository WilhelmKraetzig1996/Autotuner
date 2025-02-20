#include <complex>

#include "autotuner/ValidPeriodCalculator.h"


namespace autotuner::backend
{

    namespace
    {
        constexpr float MAX_FREQUENCY = 5000.0f;
        constexpr float MIN_FREQUENCY = 80.0f;
        constexpr size_t MAX_ALLOWED_MISDETECTIONS = 3;
        constexpr float RELATIVE_MISDETECTION_THRESHOLD = 0.01f;
    }

    ValidPeriodCalculator::ValidPeriodCalculator(std::shared_ptr<IPeriodDetector> periodDetector, std::shared_ptr<IGoalPeriodCalculator> goalPeriodCalculator, size_t samplerate)
    : periodDetector_(periodDetector)
    , goalPeriodCalculator_(goalPeriodCalculator)
    , currPeriods_(20)
    , lastInputPeriod_(0)
    , minPeriod_(static_cast<float>(samplerate) / MAX_FREQUENCY)
    , maxPeriod_(static_cast<float>(samplerate) / MIN_FREQUENCY)
    , misdetectionsFound_(0)
    {
    }

    std::tuple<float, size_t> ValidPeriodCalculator::calculatePeriods(const CircularAudioBuffer& inputBuffer, size_t blockSize)
    {
        auto inputPeriod = periodDetector_->process(inputBuffer);
        auto goalPeriod = 0.0f;
        auto inputPeriodInt = static_cast<size_t>(0);

        if (isMisdetection_(inputPeriod) && lastInputPeriod_ != 0 && misdetectionsFound_ < MAX_ALLOWED_MISDETECTIONS)
        {
            inputPeriod = lastInputPeriod_;
            misdetectionsFound_++;
            goalPeriod = goalPeriodCalculator_->calculate(inputPeriod);
            inputPeriodInt = static_cast<size_t>(inputPeriod);
        }
        else if (isValidPeriod_(inputPeriod))
        {
            misdetectionsFound_ = 0;
            lastInputPeriod_ = inputPeriod;
            goalPeriod = goalPeriodCalculator_->calculate(inputPeriod);
            inputPeriodInt = static_cast<size_t>(inputPeriod);
        }
        else
        {
            inputPeriodInt = static_cast<size_t>(blockSize);
            goalPeriod = static_cast<float>(inputPeriodInt);
            for (size_t i = 0; i < currPeriods_.size(); ++i)
                currPeriods_[i] = 0;
        }
        return {goalPeriod, inputPeriodInt};
    }

    size_t ValidPeriodCalculator::calculateNextPeriod(float goalPeriod)
    {
        auto currPeriod = static_cast<size_t>(goalPeriod);
        currPeriods_.push(currPeriod);

        auto currPeriodsSum = 0.0f;
        for (auto i = 0ul; i < currPeriods_.size(); ++i)
            if (currPeriods_[i] != 0)
                currPeriodsSum += currPeriods_[i];
        auto currPeriodsMean = currPeriodsSum / static_cast<float>(currPeriods_.size());

        if (currPeriodsMean < static_cast<float>(goalPeriod))
            currPeriod++;
        return currPeriod;
    }

    float ValidPeriodCalculator::maxPeriod()
    {
        return maxPeriod_;
    }

    bool ValidPeriodCalculator::isValidPeriod_(float period)
    {
        return period > minPeriod_ && period < maxPeriod_;
    }

    bool ValidPeriodCalculator::isMisdetection_(float inputPeriod)
    {
        return std::abs(inputPeriod - lastInputPeriod_) > inputPeriod * RELATIVE_MISDETECTION_THRESHOLD;
    }
}
