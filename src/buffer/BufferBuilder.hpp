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
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <atomic>

class BufferConsumer;
class Buffer;

class BufferBuilder
{
private:
    Buffer*                         m_buffer;
    std::atomic_int*                m_write_position_marker_ptr; // this pointer is very dangerous, it may be set null by BufferConsumer
    int                             m_cached_write_postition; // first update cached_write_position, and then update m_write_position_marker_ptr

    static std::shared_ptr<spdlog::logger>  m_logger;

public:
    BufferBuilder(Buffer* buffer);
    ~BufferBuilder();

    /* Properties */
    Buffer*                         get_buffer() {return m_buffer;}
    bool                            is_full();

    /* Write data to buffer */
    int                             append(const char* const source, int offset, int length);
    int                             append(const char* const source, int offset, int length, bool must_complete);

    /* create / delelte buffer consumer */
    std::shared_ptr<BufferConsumer> create_buffer_consumer();

    /* manipulate write_position_marker */
    int                             get_write_position() {return m_cached_write_postition;}
};


