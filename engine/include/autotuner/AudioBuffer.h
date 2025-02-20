#pragma once

#include <cstddef>


namespace autotuner::backend
{
    class AudioBuffer
    {
    public:
        struct Range
        {
            size_t start, end;
        };

        AudioBuffer(size_t size);
        AudioBuffer(float* buffer, size_t size);
        ~AudioBuffer();

        const float& operator[](size_t idx) const;
        float& operator[](size_t idx);

        const AudioBuffer operator[](Range indices) const;
        AudioBuffer operator[](Range indices);

        AudioBuffer& operator+=(const AudioBuffer& other);
        AudioBuffer& operator*=(const AudioBuffer& other);

        void copyFrom(const AudioBuffer& other);

        size_t size() const;

        float max() const;

    private:
        const size_t size_;
        float* const buffer_;
        const bool isBufferSelfOwned_;
    };
}
