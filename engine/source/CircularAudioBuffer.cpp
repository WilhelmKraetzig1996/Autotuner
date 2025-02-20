#include <cassert>
#include <cstring>

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"


namespace autotuner::backend
{
    CircularAudioBuffer::CircularAudioBuffer(size_t size)
    : buffer_(new float[size])
    , size_(size)
    , head_(0)
    {
        for (size_t i = 0; i < size_; ++i)
            buffer_[i] = 0.0f;
    }

    CircularAudioBuffer::~CircularAudioBuffer()
    {
        delete[] buffer_;
    }

    void CircularAudioBuffer::push(const AudioBuffer& buffer)
    {
        for (size_t i = 0; i < buffer.size(); ++i)
        {
            buffer_[head_] = buffer[i];
            head_ = (head_ + 1) % size_;
        }
    }

    const float& CircularAudioBuffer::operator[](size_t idx) const
    {
        assert(idx < size_);
        return buffer_[(idx + head_) % size_];
    }

    float& CircularAudioBuffer::operator[](size_t idx)
    {
        assert(idx < size_);
        return buffer_[(idx + head_) % size_];
    }

    size_t CircularAudioBuffer::size() const
    {
        return size_;
    }

    void CircularAudioBuffer::toAudioBuffer(AudioBuffer& output, size_t offset) const
    {
        assert(offset < size_);
        assert(output.size() >= size_ - offset);

        auto* rightHalfStart = buffer_ + head_;
        auto rightHalfSize = size_ - head_;
        auto* leftHalfStart = buffer_;
        auto leftHalfSize = head_;

        if (rightHalfSize > offset)
        {
            rightHalfSize -= offset;
            rightHalfStart += offset;
        }
        else if (rightHalfSize < offset)
        {
            rightHalfSize = 0;
            leftHalfSize -= offset - rightHalfSize;
            leftHalfStart += offset - rightHalfSize;
        }

        auto* outRightHalfBegin = &output[0];
        auto* outLeftHalfBegin = &output[rightHalfSize];

        std::memcpy(outRightHalfBegin, rightHalfStart, rightHalfSize * sizeof(float));
        std::memcpy(outLeftHalfBegin, leftHalfStart, leftHalfSize * sizeof(float));
    }
}
