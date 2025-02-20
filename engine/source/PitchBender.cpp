#include <algorithm>
#include <cassert>
#include <cmath>

#include "autotuner/AudioBuffer.h"
#include "autotuner/IValidPeriodCalculator.h"
#include "autotuner/PitchBender.h"


namespace autotuner::backend
{
    PitchBender::PitchBender(size_t blockSize, size_t inputBufferSize, std::shared_ptr<IValidPeriodCalculator> periodCalculator)
    : inputBuffer_(inputBufferSize)
    , outputBuffer_(inputBuffer_.size())
    , periodCalculator_(periodCalculator)
    , inputStart_(static_cast<int>(blockSize) * 4)
    , outputStart_(0)
    , window_(std::max(static_cast<size_t>(periodCalculator_->maxPeriod()) * 2 + 1, blockSize * 2))
    {}

    void PitchBender::process(const AudioBuffer& input, AudioBuffer& output)
    {

        inputBuffer_.push(input);
        outputBuffer_.push(output);
        auto currBlockSize = input.size();

        auto [goalPeriod, inputPeriodInt] = periodCalculator_->calculatePeriods(inputBuffer_, currBlockSize);

        updateWindow_(inputPeriodInt);

        auto nOutputPeriods = calculateNumOutputPeriods_(currBlockSize, goalPeriod);
        auto inputBufferNeeded = nOutputPeriods * inputPeriodInt + inputPeriodInt;
        assert(inputBufferNeeded <= inputBuffer_.size());

        wrapInputStartToValidBoundaries_(inputPeriodInt, inputBufferNeeded);

        for (auto iPeriod = 0ul; iPeriod < nOutputPeriods; ++iPeriod)
        {
            auto currPeriod = periodCalculator_->calculateNextPeriod(goalPeriod);
            for (auto i = 0ul; i < 2 * inputPeriodInt; ++i)
                outputBuffer_[static_cast<size_t>(outputStart_) + i] += inputBuffer_[static_cast<size_t>(inputStart_) + i] * window_[i];
            outputStart_ += static_cast<int>(currPeriod);
            inputStart_ += static_cast<int>(inputPeriodInt);
        }
        outputStart_ -= static_cast<int>(currBlockSize);
        inputStart_ -= static_cast<int>(currBlockSize);

        for (auto i = 0ul; i < currBlockSize; ++i)
            output[i] = outputBuffer_[i];
    }

    void PitchBender::wrapInputStartToValidBoundaries_(size_t inputPeriod, size_t inputBufferNeeded)
    {
        while (inputStart_ > static_cast<int>(inputBuffer_.size() - inputBufferNeeded))
            inputStart_ -= static_cast<int>(inputPeriod);
        while (inputStart_ < 0)
            inputStart_ += static_cast<int>(inputPeriod);
    }

    size_t PitchBender::calculateNumOutputPeriods_(size_t blockSize, float goalPeriod)
    {
        auto nOutputPeriods = static_cast<size_t>(std::ceil(((static_cast<float>(blockSize) - static_cast<float>(outputStart_)) / static_cast<float>(goalPeriod))));
        if (outputStart_ > static_cast<int>(blockSize))
            nOutputPeriods = 0;
        return nOutputPeriods;
    }

    void PitchBender::updateWindow_(size_t period)
    {
        auto step = 1.0f / static_cast<float>(period);
        for (size_t i = 0; i < period; ++i)
        {
            window_[i] = static_cast<float>(i) * step;
            window_[i + period] = 1 - static_cast<float>(i) * step;
        }
    }
}
