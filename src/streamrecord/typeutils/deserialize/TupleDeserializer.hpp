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

class TupleDeserializer: public TypeDeserializer
{ 
private:
    std::shared_ptr<TypeDeserializerImpl>       m_type_deserializer;

    int                                         m_record_fields;
    int                                         m_record_size;
    int                                         m_remaining;  // tuple buffer left to read
    static std::shared_ptr<spdlog::logger>      m_logger;

public:
    TupleDeserializer() {
        m_type_deserializer = std::make_shared<TypeDeserializerImpl>();
        m_record_fields = -1; // no record at the creation of deserializer.
    }

    void                                        set_next_buffer(std::shared_ptr<BufferBase> buffer) {m_type_deserializer->set_next_buffer(buffer);}
    DeserializationResult                       get_next_record(std::shared_ptr<IOReadableWritable> target);

    int                                         read_short() {return m_type_deserializer->read_short();}
    int                                         read_int() {return m_type_deserializer->read_int();}
    double                                      read_double() {return m_type_deserializer->read_double();}
    int                                         read_byte() {return m_type_deserializer->read_byte();}
    int                                         read_unsigned_byte() {return m_type_deserializer->read_unsigned_byte();}
    void                                        read_unsigned_bytes(unsigned char * buf, int length) {m_type_deserializer->read_unsigned_bytes(buf, length);}

    /* Properties */
    int                                         get_record_size() {return m_record_size;}
    void                                        set_record_size(int record_size) {m_record_size = record_size;}
    std::string                                 dump_state() {} 
};


