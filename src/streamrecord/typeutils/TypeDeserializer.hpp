/**
 * Deserializer of a buffer. The deserializer is binded to an input channel. No thread safe guarantee.
 */

#pragma once
#include "../../buffer/BufferBase.hpp"
#include "../types/IOReadableWritable.hpp"
#include "../StreamRecord.hpp"
#include "SerializeUtils.hpp"
#include <iostream>
#include <memory>
#include <deque>

class IOReadableWritable;

class TypeDeserializer
{
private:
    std::deque<BufferBase*>     m_last_buffers; // m_last_buffers caches all incomplete buffer, 
                                                                // waiting for the last buffer to revive the whole object
    int                                         m_record_size;

    int                                         m_position;
    int                                         m_remaining;
             
public:
    TypeDeserializer() {
        m_record_size = -1; // no record at the creation of deserializer.
        m_position = -1;
        m_remaining = 0;
    }
    void                                        set_next_buffer(BufferBase* buffer);
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
};

