/**
 * Deserializer of a buffer. The deserializer is binded to an input channel. No thread safe guarantee.
 */

#pragma once
#include "../../buffer/BufferBase.hpp"
#include "../types/IOReadableWritable.hpp"
#include "../StreamRecord.hpp"
#include "SerializeUtils.hpp"
#include "Constant.hpp"
#include <iostream>
#include <memory>
#include <deque>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

class IOReadableWritable;

class TypeDeserializer
{
private:
    std::deque<std::shared_ptr<BufferBase>>     m_last_buffers; // m_last_buffers caches all incomplete buffer, 
                                                                // waiting for the last buffer to revive the whole object
    int                                         m_record_size;

    int                                         m_position;
    int                                         m_remaining;
    static std::shared_ptr<spdlog::logger>      m_logger;
             
public:
    TypeDeserializer() {
        m_record_size = -1; // no record at the creation of deserializer.
        m_position = -1;
        m_remaining = 0;
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
    }
    void                                        set_next_buffer(std::shared_ptr<BufferBase> buffer);
    DeserializationResult                       get_next_record(std::shared_ptr<IOReadableWritable> target);

    int                                         read_short();
    int                                         read_int();
    double                                      read_double();
    int                                         read_byte();
    int                                         read_unsigned_byte();
    DeserializationResult                       read_into(std::shared_ptr<IOReadableWritable> target);

    void                                        evict_used_buffer(bool is_finish_read);
    void                                        check_end_with_one_byte();

    /* Properties */
    int                                         get_record_size() {return m_record_size;}
    std::string                                 dump_state() {return "number of buffers: " + std::to_string(m_last_buffers.size()) + 
                                                                    ", read position: " + std::to_string(m_position) + 
                                                                    ", remaining buffers: " + std::to_string(m_remaining);}
};

