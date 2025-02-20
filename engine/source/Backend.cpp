#include "autotuner/Backend.h"
#include "autotuner/GoalPeriodCalculator.h"
#include "autotuner/PeriodDetector.h"


namespace autotuner::backend
{
    namespace
    {
        constexpr float MIN_FREQUENCY = 80.0f;
    }

    Backend::Backend(size_t blockSize, size_t sampleRate, GoalPeriodCalculator::Key key)
    : goalPeriodCalculator_(std::make_shared<GoalPeriodCalculator>(sampleRate, key))
    {
        auto periodDetectionLength = static_cast<size_t>(1.5 * static_cast<float>(sampleRate) / MIN_FREQUENCY);
        auto inputBufferSize = std::max(blockSize * 4, 2 * periodDetectionLength);

        periodDetector_ = std::make_shared<PeriodDetector>(inputBufferSize, periodDetectionLength);
        validPeriodCalculator_ = std::make_shared<ValidPeriodCalculator>(periodDetector_, goalPeriodCalculator_, sampleRate);
        pitchBender = std::make_unique<PitchBender>(blockSize, inputBufferSize, validPeriodCalculator_);
    }


    void Backend::process(const AudioBuffer& input, AudioBuffer& output)
    {
        pitchBender->process(input, output);
    }

    void Backend::setKey(GoalPeriodCalculator::Key key)
    {
        goalPeriodCalculator_->setKey(key);
    }

    void Backend::setMinimumPeriodProbability(float probability)
    {
        periodDetector_->setMinimumPeriodProbability(probability);
    }
}
