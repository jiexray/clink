/**
 * BufferConsumer read data written by BufferBuilder. 
 */
#pragma once

#include "Buffer.hpp"
#include "BufferBase.hpp"
#include <iostream>
#include <atomic>

class BufferConsumer
{
private:
    Buffer*             m_buffer;
    std::atomic_int*    m_write_position_marker_ptr;
    int                 m_read_position_marker;
    bool                m_is_finished;
public:
    BufferConsumer(Buffer* buffer, std::atomic_int* current_write_position_ptr, int current_read_position);
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

inline BufferConsumer::BufferConsumer(Buffer* buffer, std::atomic_int* current_write_position_ptr, int current_read_position):
m_buffer(buffer), 
m_write_position_marker_ptr(current_write_position_ptr), 
m_read_position_marker(0){}

inline BufferBase* BufferConsumer::build() {
    if (m_read_position_marker == *m_write_position_marker_ptr) {
        // current buffer is empty
        return nullptr;
    }
    /**
     * Note: we can only read writer position marker one time using atomic read.
     *       (1) store the previous read position;
     *       (2) get the target read postition
     *       (3) return the buffer slice between the old and target read positions.
     */
    int old_read_position = m_read_position_marker;
    m_read_position_marker = m_write_position_marker_ptr->load();
    BufferBase* bufferSlice = m_buffer->read_only_slice(old_read_position, m_read_position_marker - old_read_position);

    // check finish and clear write_position_marker
    if (m_read_position_marker == m_buffer->get_max_capacity()) {
        free(m_write_position_marker_ptr);
        m_write_position_marker_ptr = nullptr;
    }

    return bufferSlice;
}