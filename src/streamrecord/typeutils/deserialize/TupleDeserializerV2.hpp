/**
 * Deserialize a tuple from buffer to Tuple.
 * 
 * A TupleDeserializer continaer a TypeDeserializer
 */
#pragma once
#include "TypeDeserializer.hpp"
#include "TypeDeserializerImpl.hpp"
#include "TupleDeserializationDelegate.hpp"
#include "Tuple.hpp"
#include "Tuple2.hpp"
#include "LoggerFactory.hpp"
#include <typeinfo>
#include <string.h>

class TupleDeserializerV2: public TypeDeserializer, public std::enable_shared_from_this<TupleDeserializerV2>
{ 
private:
    std::shared_ptr<TypeDeserializerImpl>       m_type_deserializer;

    int                                         m_record_fields;
    int                                         m_record_size;
    int                                         m_remaining;  // tuple buffer left to read
    static std::shared_ptr<spdlog::logger>      m_logger;

public:
    TupleDeserializerV2() {
        m_type_deserializer = std::make_shared<TypeDeserializerImpl>();
        m_record_fields = -1; // no record at the creation of deserializer.
        m_remaining = 0;
    }

    void                                        set_next_buffer(std::shared_ptr<BufferBase> buffer) {
        m_type_deserializer->set_next_buffer(buffer);
        m_remaining += buffer->get_max_capacity();
    }
    DeserializationResult                       get_next_record(std::shared_ptr<IOReadableWritable> target);

    int                                         read_short() {
        m_remaining -= 2;
        return m_type_deserializer->read_short();
    }
    int                                         read_int() {
        m_remaining -= 4;
        return m_type_deserializer->read_int();
    }
    double                                      read_double() {
        m_remaining -= 8;
        return m_type_deserializer->read_double();
    }
    int                                         read_byte() {
        m_remaining -= 1;
        return m_type_deserializer->read_byte();
    }
    int                                         read_unsigned_byte() {
        m_remaining -= 1;
        return m_type_deserializer->read_unsigned_byte();
    }
    void                                        read_unsigned_bytes(unsigned char * buf, int length) {
        m_remaining -= length;
        m_type_deserializer->read_unsigned_bytes(buf, length);
    }

    void                                        read_unsigned_bytes_no_copy(unsigned char** buf, int length) {
        m_remaining -= length;
        m_type_deserializer->read_unsigned_bytes_no_copy(buf, length);
    }
    void                                        read_commit() {
        m_type_deserializer->read_commit();
    }

    /* Properties */
    int                                         get_record_size() {return m_record_size;}
    void                                        set_record_size(int record_size) {m_record_size = record_size;}
    std::string                                 dump_state() {
        return "TypeDeserialize state: " + m_type_deserializer->dump_state() + ", TupleDeserializer remaining: " + std::to_string(m_remaining);
    } 
};


