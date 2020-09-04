/**
 * BufferBuilder provides API for filling the content of Buffer.
 * For access the data in Buffer, use BufferConsumer.
 * 
 * We assume only one thread can get access to BufferBuilder.
 * BufferConsumer may be prossessed by another thread. However, BufferConsumer can only read the write_position_marker.
*/
#pragma once

#include "Buffer.hpp"
#include "BufferConsumer.hpp"
#include <memory>
#include <stdexcept>
#include <algorithm>

class BufferBuilder
{
private:
    Buffer*                         m_buffer;
    // TODO: the writer marker should be a atomic value, both builder and consumer from two different threads 
    //       may get access to this value.
    int*                            m_write_position_marker_ptr;

public:
    BufferBuilder(Buffer* buffer);
    ~BufferBuilder() {}

    /* Properties */
    int                             get_write_position() {return *m_write_position_marker_ptr;}
    Buffer*                         get_buffer() {return m_buffer;}
    bool                            is_full() {return m_buffer->get_max_capacity() == *m_write_position_marker_ptr;}

    /* Write data to buffer */
    int                             append(const char* const source, int offset, int length);
    int                             append(const char* const source, int offset, int length, bool must_complete);

    /* create / delelte buffer consumer */
    std::shared_ptr<BufferConsumer> create_buffer_consumer();
    void                            recycle_buffer_consumer();
};


inline BufferBuilder::BufferBuilder(Buffer* buffer):
m_buffer(buffer) {m_write_position_marker_ptr = new int{0};}

inline int BufferBuilder::append(const char* const source, int offset, int length) {
    int buffer_capacity = m_buffer->get_max_capacity();
    int available = buffer_capacity - *m_write_position_marker_ptr;
    int to_copy = std::min(available, length);

    for (int i = 0; i < to_copy; i++) {
        m_buffer->put((*m_write_position_marker_ptr)++, source[offset + i]);
    }
    return to_copy;
}

inline int BufferBuilder::append(const char* const source, int offset, int length, bool must_complete){
    if (!must_complete) {
        return append(source, offset, length);
    } else {
        int buffer_capacity = m_buffer->get_max_capacity();
        int available = buffer_capacity - *m_write_position_marker_ptr;
        if (available < length) {
            // force to fill the unfinished buffer with fake chars
            for (int i = 0; i < available; i++) {
                m_buffer->put((*m_write_position_marker_ptr)++, (char)0);
            }
            return 0;
        } else {
            return append(source, offset, length);
        }
    }
}

inline std::shared_ptr<BufferConsumer> BufferBuilder::create_buffer_consumer() {
    return std::make_shared<BufferConsumer>(m_buffer, m_write_position_marker_ptr, 0);
    // return std::make_shared<BufferConsumer>(m_buffer, m_write_position_marker, 0);
}


inline void BufferBuilder::recycle_buffer_consumer() {
    // TODO: recycle buffer consumer before free buffer builder
}