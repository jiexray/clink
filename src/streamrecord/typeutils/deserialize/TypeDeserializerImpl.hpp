/**
 * Deserializer of a buffer. The deserializer is binded to an input channel. No thread safe guarantee.
 */

#pragma once
#include "BufferBase.hpp"
#include "IOReadableWritable.hpp"
#include "StreamRecord.hpp"
#include "TypeDeserializer.hpp"
#include "SerializeUtils.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <iostream>
#include <memory>
#include <deque>
#include <mutex>
#include <atomic>

class IOReadableWritable;

class TypeDeserializerImpl: public TypeDeserializer, public std::enable_shared_from_this<TypeDeserializerImpl>
{
private:
    std::deque<std::shared_ptr<BufferBase>>     m_last_buffers; // m_last_buffers caches all incomplete buffer, 
                                                                // waiting for the last buffer to revive the whole object
    int                                         m_record_size;

    int                                         m_position;
    int                                         m_remaining;
    static std::shared_ptr<spdlog::logger>      m_logger;

public:
    TypeDeserializerImpl() {
        m_record_size = -1; // no record at the creation of deserializer.
        m_position = -1;
        m_remaining = 0;
    }
    void                                        set_next_buffer(std::shared_ptr<BufferBase> buffer);
    virtual DeserializationResult               get_next_record(std::shared_ptr<IOReadableWritable> target);

    int                                         read_short();
    int                                         read_int();
    double                                      read_double();
    int                                         read_byte();
    int                                         read_unsigned_byte();
    void                                        read_unsigned_bytes(unsigned char * buf, int length);
    DeserializationResult                       read_into(std::shared_ptr<IOReadableWritable> target);

    /* read with no buffer copy */
    void                                        read_unsigned_bytes_no_copy(unsigned char** buf, int length);
    void                                        read_commit();

    void                                        evict_used_buffer(bool is_finish_read);

    /* Properties */
    int                                         get_record_size() {return m_record_size;}
    void                                        set_record_size(int record_size) {m_record_size = record_size;}
    std::string                                 dump_state() {return "number of buffers: " + std::to_string(m_last_buffers.size()) + 
                                                                    ", read position: " + std::to_string(m_position) + 
                                                                    ", remaining buffers: " + std::to_string(m_remaining);}
    int                                         get_remaining() {return m_remaining;}
};

