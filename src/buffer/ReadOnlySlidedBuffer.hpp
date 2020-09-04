/**
 * Read-only sliced Buffer implementation wrapping a Buffer's sub-region.
 */
#pragma once

#include "Buffer.hpp"
#include "BufferBase.hpp"

class ReadOnlySlidedBuffer : public BufferBase
{
private:
    int         m_buffer_offset;
    int         m_size;

    /* parent buffer */
    BufferBase* parent_buffer;
public:
    ReadOnlySlidedBuffer(BufferBase* buffer, int offset, int length);
    ~ReadOnlySlidedBuffer() {};

    /* Slice the buffer, override from BufferBase */
    int         get_buffer_offset()                     override;
    BufferBase* read_only_slice(int offset, int length) override;

    /* Properties */
    int         get_max_capacity()                      override {return m_size;}

    /* Random access to data */
    int         get(char* buf, int index)                          override;
};

inline ReadOnlySlidedBuffer::ReadOnlySlidedBuffer(BufferBase* buffer, int offset, int length):
parent_buffer(buffer),
m_buffer_offset(offset),
m_size(length){}

inline int ReadOnlySlidedBuffer::get_buffer_offset() {
    return m_buffer_offset;
}

inline BufferBase* ReadOnlySlidedBuffer::read_only_slice(int offset, int length) {
    if (offset >= m_size || offset + length > m_size) {
        std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
         "], capacity [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    parent_buffer->read_only_slice(m_buffer_offset + offset, length);
}

inline int ReadOnlySlidedBuffer::get(char* buf, int index) {
    if (index >= m_size) {
        std::cerr << "index [" << index << "] is out of the range [" << m_size << "]" << std::endl;
        return -1; 
    }
    parent_buffer->get(buf,m_buffer_offset + index);
}