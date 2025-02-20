#pragma once

#include <memory>

#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/IGoalPeriodCalculator.h"
#include "autotuner/IPeriodDetector.h"
#include "autotuner/IValidPeriodCalculator.h"


namespace autotuner::backend
{
    class ValidPeriodCalculator : public IValidPeriodCalculator
    {
    public:
        ValidPeriodCalculator(std::shared_ptr<IPeriodDetector> periodDetector, std::shared_ptr<IGoalPeriodCalculator> goalPeriodCalculator, size_t samplerate);
        ~ValidPeriodCalculator() override = default;

        std::tuple<float, size_t> calculatePeriods(const CircularAudioBuffer& inputBuffer, size_t blockSize) override;
        size_t calculateNextPeriod(float goalPeriod) override;
        float maxPeriod() override;

    private:
        bool isValidPeriod_(float period);
        bool isMisdetection_(float inputPeriod);

        std::shared_ptr<IPeriodDetector> periodDetector_;
        std::shared_ptr<IGoalPeriodCalculator> goalPeriodCalculator_;

        CircularAudioBuffer currPeriods_;
        float lastInputPeriod_;
        float minPeriod_;
        float maxPeriod_;

        size_t misdetectionsFound_;
    };


}
