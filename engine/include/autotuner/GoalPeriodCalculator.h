#pragma once

#include "autotuner/IGoalPeriodCalculator.h"
#include <sys/types.h>


namespace autotuner::backend
{

    class GoalPeriodCalculator : public IGoalPeriodCalculator
    {
    public:
        enum Key
        {
            A_MAJOR = 0,
            B_FLAT_MAJOR = 1,
            B_MAJOR = 2,
            C_MAJOR = 3,
            C_SHARP_MAJOR = 4,
            D_MAJOR = 5,
            E_FLAT_MAJOR = 6,
            E_MAJOR = 7,
            F_MAJOR = 8,
            F_SHARP_MAJOR = 9,
            G_MAJOR = 10,
            A_FLAT_MAJOR = 11,
        };

        GoalPeriodCalculator(size_t samplerate, Key goalKey);
        float calculate(float inputPeriod) override;
        void setKey(Key key);


    private:
        class ToneLut
        {
        public:
            ToneLut(size_t size);
            ~ToneLut();
            const float& operator[](size_t idx) const;
            float& operator[](size_t idx);
            size_t size() const;

        private:
            size_t size_;
            float* lut_;
        };

        size_t samplerate_;
        ToneLut lut_;
    };
}
