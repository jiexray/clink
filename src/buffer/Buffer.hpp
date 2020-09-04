/**
 * Buffer holds the data block of multiple tuples, which need to be transmitted between consequtive tasks.
 */
#pragma once

#include <string>
#include <iostream>
#include "ReadOnlySlidedBuffer.hpp"
#include "BufferBase.hpp"
#include <stdexcept>

class Buffer : public BufferBase
{
private:
    char*       m_data;
    int         m_size;

public:
    Buffer();
    Buffer(int capacity);
    ~Buffer() {};

    void    free();

    /* Properties */
    int     get_max_capacity() override {return m_size;}

    /* Random Access get() and put() methods */
    int      get(char* buf, int index) override;
    void     put(int index, char value);

    /* Slice the buffer, override from BufferBase */
    int get_buffer_offset() override {return 0;}
    BufferBase* read_only_slice(int offset, int length) override;
};

/**
 * Create a new buffer, allocate new memory.
 */
inline Buffer::Buffer(int capacity) {
    //TODO: OOM problem check
    m_data      = new char[capacity];
    m_size      = capacity;
}

inline int Buffer::get(char* buf, int index) {
    if (index >= m_size) {
        // std::cerr << "index [" << index << "] is out of the range [" << m_size << "]" << std::endl;
        return -1;
    }
    (*buf) = m_data[index];
    return 1;
}

inline void Buffer::put(int index, char value) {
    if (index >= m_size) {
        std::string error_info = "index [" + std::to_string(index) + "] is out of the range [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    m_data[index] = value;
}

inline BufferBase* Buffer::read_only_slice(int offset, int length) {
    if (offset >= m_size || offset + length > m_size) {
        std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
         "], capacity [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    return new ReadOnlySlidedBuffer(this, offset, length);
}
