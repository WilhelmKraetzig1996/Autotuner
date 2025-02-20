#pragma once

#include "autotuner/AudioBuffer.h"


namespace autotuner::backend
{
    class CircularAudioBuffer
    {
    public:
        CircularAudioBuffer(size_t size);
        ~CircularAudioBuffer();

        const float& operator[](size_t idx) const;
        float& operator[](size_t idx);

        void push(const AudioBuffer& buffer);

        size_t size() const;

        void toAudioBuffer(AudioBuffer& output, size_t offset = 0) const;

    private:
        float* const buffer_;
        const size_t size_;
        size_t head_;
    };

}
