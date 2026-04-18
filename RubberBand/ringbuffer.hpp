// Workflow:
// 1. Make a RingBuffer with specified input and output block sizes, and padding coefficient.
//    The actual buffer size is (input_size + output_size) * padding_coef. The padding coefficient
//    makes so that the buffer is bigger than necessary to prevent overwrites while reading.
// 2. Initialize the memory separately and provide the allocated buffer with initialize().
//    Call the size() method first to determine how big the buffer should be that you need to allocate.
// 3. You can call writeBlock() each time you have a new block of samples to write to the buffer.
// 4. Before reading, you should call isReadReady(). This tells you if a new block of samples
//    is available to read. Then you can call readBlock().

#pragma once
#include <cstddef>

template <typename T>
class TestRingBuffer;

template <typename T>
class RingBuffer {
public:
    RingBuffer<T>(size_t inputBlockSize, size_t outputBlockSize, size_t paddingCoef);
    size_t getInputBlockSize();
    size_t getOutputBlockSize();
    void initialize(T* buffer);
    bool isReadReady();
    void readBlock(T* destination);
    size_t size();
    void writeBlock(T* samples, size_t length);
    T* m_buffer;
private:
    size_t m_size; // size of buffer overall
    size_t m_inputBlockSize; // input block size
    size_t m_outputBlockSize; // output block size
    size_t m_inputPointer; // input write pointer
    size_t m_outputPointer; // output read pointer
    size_t m_newSamples; // number of new samples accumulated since last read
    template <typename U>
    friend class TestRingBuffer;
};

template <typename T>
RingBuffer<T>::RingBuffer(size_t inputBlockSize, size_t outputBlockSize, size_t paddingCoef) {
    m_buffer = nullptr;
    m_inputBlockSize = inputBlockSize;
    m_outputBlockSize = outputBlockSize;
    m_size = (inputBlockSize + outputBlockSize) * paddingCoef;
    m_newSamples = 0;
    m_inputPointer = 0;
    m_outputPointer = 0;
}

template <typename T>
size_t RingBuffer<T>::getInputBlockSize() {
    return m_inputBlockSize;
}

template <typename T>
size_t RingBuffer<T>::getOutputBlockSize() {
    return m_outputBlockSize;
}

template <typename T>
void RingBuffer<T>::initialize(T* buffer) {
    m_buffer = buffer;
}

template <typename T>
void RingBuffer<T>::writeBlock(T* samples, size_t length) {
    if (m_buffer && m_size > 0) {
        size_t startPos = m_inputPointer;
        for (size_t i = 0; i < length; i++) {
            m_buffer[m_inputPointer] = samples[i];
            m_inputPointer++;
            if (m_inputPointer >= m_size) {
                m_inputPointer = 0;
            }
            m_newSamples++;
        }
    }
}

template <typename T>
void RingBuffer<T>::readBlock(T* destination) {
    if (m_buffer && m_size > 0) {
        for (size_t i = 0; i < m_outputBlockSize; i++) {
            destination[i] = m_buffer[m_outputPointer];
            m_outputPointer++;
            if (m_outputPointer >= m_size) {
                m_outputPointer = 0;
            }
        }
        // cannot allow the number of new samples to go negative
        if (m_newSamples < m_outputBlockSize) {
            m_newSamples = 0;
        } else {
            m_newSamples -= m_outputBlockSize;
        }
    }
}

template <typename T>
bool RingBuffer<T>::isReadReady() {
    if (m_newSamples >= m_outputBlockSize) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
size_t RingBuffer<T>::size() {
    return m_size;
}