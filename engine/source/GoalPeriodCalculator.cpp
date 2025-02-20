#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "autotuner/GoalPeriodCalculator.h"


namespace autotuner::backend
{
    namespace
    {
        constexpr size_t N_OCTAVES = 8;
        constexpr float STANDARD_PITCH = 440.0f;
        constexpr float ROOT_OCTAVE = STANDARD_PITCH / 16;
        constexpr size_t N_SEMITONES_IN_OCTAVE = 12;
        constexpr std::array<size_t, 7> MAJOR_KEY_INDICES = {0, 2, 4, 5, 7, 9, 11};
    }

    GoalPeriodCalculator::GoalPeriodCalculator(size_t samplerate, Key goalKey)
    : samplerate_(samplerate)
    , lut_(MAJOR_KEY_INDICES.size() * N_OCTAVES)
    {
        setKey(goalKey);
    }

    void GoalPeriodCalculator::setKey(Key goalKey)
    {
        auto keyIndices = MAJOR_KEY_INDICES;
        for (auto& keyIndex: keyIndices)
        {
            keyIndex += static_cast<size_t>(goalKey);
            keyIndex %= N_SEMITONES_IN_OCTAVE;
        }
        std::sort(keyIndices.begin(), keyIndices.end());
        for (auto i_octave = 0ul; i_octave < N_OCTAVES; i_octave++)
            for (auto iKey = 0ul; iKey < keyIndices.size(); iKey++)
            {
                auto lutIndex = iKey + MAJOR_KEY_INDICES.size() * i_octave;
                auto octaveFactor = std::pow(2.0f, static_cast<float>(i_octave));
                auto semitoneFactor = std::pow(std::pow(2.0f, static_cast<float>(keyIndices[iKey])), 1.f / 12.f);
                lut_[lutIndex] = static_cast<float>(samplerate_) / (ROOT_OCTAVE * octaveFactor * semitoneFactor);
            }
    }

    float GoalPeriodCalculator::calculate(float inputPeriod)
    {

        auto iLowerBound = 0ul;
        while (iLowerBound < lut_.size() - 1 && lut_[iLowerBound + 1] > inputPeriod)
            iLowerBound++;

        auto iHigherBound = iLowerBound + 1;

        if (iHigherBound == lut_.size())
            return lut_[iLowerBound];

        if (lut_[iHigherBound] - inputPeriod > inputPeriod - lut_[iLowerBound])
            return lut_[iHigherBound];

        return lut_[iLowerBound];
    }


    GoalPeriodCalculator::ToneLut::ToneLut(size_t size)
    : size_(size)
    , lut_(new float[size])
    {}

    GoalPeriodCalculator::ToneLut::~ToneLut()
    {
        delete[] lut_;
    }

    const float& GoalPeriodCalculator::ToneLut::operator[](size_t idx) const
    {
        assert(idx < size_);
        return lut_[idx];
    }

    float& GoalPeriodCalculator::ToneLut::operator[](size_t idx)
    {
        assert(idx < size_);
        return lut_[idx];
    }

    size_t GoalPeriodCalculator::ToneLut::size() const
    {
        return size_;
    }
}
