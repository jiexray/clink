/**
 * BufferConsumer read data written by BufferBuilder. 
 */
#pragma once

#include "Buffer.hpp"
#include "BufferBase.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <iostream>
#include <atomic>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <sstream>

class Buffer;

class BufferConsumer
{
private:
    Buffer*                         m_buffer;
    std::atomic_int*                m_write_position_marker_ptr;
    int                             m_read_position_marker;
    bool                            m_is_finished;

    static std::shared_ptr<spdlog::logger> m_logger;
public:
    BufferConsumer(Buffer* buffer, std::atomic_int* current_write_position_ptr, int current_read_position);
    ~BufferConsumer();

    /* Properties */
    int                             get_read_position() {return m_read_position_marker;}
    int                             get_write_position();
    bool                            is_finished();
    bool                            is_data_available();

    // BufferBase*     build();
    std::shared_ptr<BufferBase>     build();
};