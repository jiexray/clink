/**
 * BufferConsumer read data written by BufferBuilder. 
 */
#pragma once

#include "Buffer.hpp"
#include "BufferBase.hpp"
#include <iostream>

class BufferConsumer
{
private:
    Buffer*         m_buffer;
    // TODO: change int to atomic int
    int*            m_write_position_marker_ptr;
    int             m_read_position_marker;
    bool            m_is_finished;
public:
    BufferConsumer(Buffer* buffer, int* current_write_position_ptr, int current_read_position);
    ~BufferConsumer() {};

    /* Properties */
    int             get_read_position() {return m_read_position_marker;}
    int             get_write_position();
    bool            is_finished();
    bool            is_data_available();

    BufferBase*     build();
};

inline bool BufferConsumer::is_data_available() {
    if (m_write_position_marker_ptr == nullptr) {
        return false;
    }
    return m_read_position_marker < *m_write_position_marker_ptr;
}

inline bool BufferConsumer::is_finished() {
    if (m_write_position_marker_ptr == nullptr) {
        return true;
    }
    return *m_write_position_marker_ptr == m_buffer->get_max_capacity();
}

inline int BufferConsumer::get_write_position() {
    if (m_write_position_marker_ptr == nullptr) {
        return -1;
    }
    return *m_write_position_marker_ptr;
}

inline BufferConsumer::BufferConsumer(Buffer* buffer, int* current_write_position_ptr, int current_read_position):
m_buffer(buffer), 
m_write_position_marker_ptr(current_write_position_ptr), 
m_read_position_marker(0){}

inline BufferBase* BufferConsumer::build() {
    if (m_read_position_marker == *m_write_position_marker_ptr) {
        // current buffer is empty
        return nullptr;
    }
    BufferBase* bufferSlice = m_buffer->read_only_slice(m_read_position_marker, *m_write_position_marker_ptr - m_read_position_marker);
    // TODO: This procedure is not thread-safe
    m_read_position_marker = *m_write_position_marker_ptr;
    if (is_finished()) {
        // free the m_writer_position_marker_ptr
        free(m_write_position_marker_ptr);
        m_write_position_marker_ptr = nullptr;
    }
    return bufferSlice;
}