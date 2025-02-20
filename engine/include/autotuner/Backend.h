#pragma once

#include "autotuner/GoalPeriodCalculator.h"
#include "autotuner/PeriodDetector.h"
#include "autotuner/PitchBender.h"
#include "autotuner/ValidPeriodCalculator.h"


namespace autotuner::backend
{
    class Backend
    {
    public:
        Backend(size_t blockSize, size_t sampleRate, GoalPeriodCalculator::Key key);

        void process(const AudioBuffer& input, AudioBuffer& output);

        void setKey(GoalPeriodCalculator::Key key);

        void setMinimumPeriodProbability(float probability);

    private:
        std::shared_ptr<GoalPeriodCalculator> goalPeriodCalculator_;
        std::shared_ptr<PeriodDetector> periodDetector_;
        std::shared_ptr<ValidPeriodCalculator> validPeriodCalculator_;
        std::unique_ptr<PitchBender> pitchBender;
    };
}
