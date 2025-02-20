#include <cassert>

#include "autotuner/AudioBuffer.h"


namespace autotuner::backend
{
    AudioBuffer::AudioBuffer(size_t size)
    : size_(size)
    , buffer_(new float[size])
    , isBufferSelfOwned_(true)
    {
        for (size_t i = 0; i < size; i++)
            buffer_[i] = 0.0f;
    }

    AudioBuffer::AudioBuffer(float* buffer, size_t size)
    : size_(size)
    , buffer_(buffer)
    , isBufferSelfOwned_(false)
    {
        assert(buffer != nullptr);
    }

    AudioBuffer::~AudioBuffer()
    {
        if (isBufferSelfOwned_)
            delete[] buffer_;
    }

    const float& AudioBuffer::operator[](size_t idx) const
    {
        assert(idx < size_);
        return buffer_[idx];
    }

    float& AudioBuffer::operator[](size_t idx)
    {
        assert(idx < size_);
        return buffer_[idx];
    }


    AudioBuffer AudioBuffer::operator[](AudioBuffer::Range indices)
    {
        return AudioBuffer(buffer_ + indices.start, indices.end - indices.start + 1);
    }

    const AudioBuffer AudioBuffer::operator[](AudioBuffer::Range indices) const
    {
        return AudioBuffer(buffer_ + indices.start, indices.end - indices.start + 1);
    }

    AudioBuffer& AudioBuffer::operator+=(const AudioBuffer& other)
    {
        assert(size_ == other.size());

        for (size_t i = 0; i < size_; i++)
            buffer_[i] += other[i];

        return *this;
    }

    AudioBuffer& AudioBuffer::operator*=(const AudioBuffer& other)
    {
        assert(size_ == other.size());

        for (size_t i = 0; i < size_; i++)
            buffer_[i] *= other[i];

        return *this;
    }

    void AudioBuffer::copyFrom(const AudioBuffer& other)
    {
        assert(size_ == other.size());
        for (size_t i = 0; i < size_; i++)
            buffer_[i] = other[i];
    }

    size_t AudioBuffer::size() const
    {
        return size_;
    }

    float AudioBuffer::max() const
    {
        float maximum = buffer_[0];
        for (size_t i = 1; i < size_; i++)
            if (buffer_[i] > maximum)
                maximum = buffer_[i];
        return maximum;
    }
}
