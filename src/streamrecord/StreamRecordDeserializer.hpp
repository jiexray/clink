/**
 * Interface for turning sequences of memory segments into record.
 */
#pragma once
#include "../../buffer/BufferBase.hpp"
#include "typeutils/TypeDeserializer.hpp"
#include "types/IOReadableWritable.hpp"
#include "StreamRecord.hpp"
#include "TypeDeserializerImpl.hpp"
#include "DeserializationDelegate.hpp"
#include "TupleDeserializationDelegate.hpp"
#include "TupleDeserializer.hpp"
#include <memory>

enum RECORD_TYPE {
    TUPLE, NORMAL
};

class StreamRecordDeserializer
{
private:
    std::shared_ptr<TypeDeserializer>   m_deserializer;
    std::shared_ptr<TupleDeserializer>  m_tuple_deserializer;

    RECORD_TYPE                         m_record_type;
public:
    StreamRecordDeserializer(): StreamRecordDeserializer(RECORD_TYPE::NORMAL) {}

    StreamRecordDeserializer(RECORD_TYPE record_type): m_record_type(record_type) {
        switch (record_type) {
        case RECORD_TYPE::TUPLE:
            m_tuple_deserializer = std::make_shared<TupleDeserializer>();
            m_deserializer.reset();
            break;
        case RECORD_TYPE::NORMAL:
            m_deserializer = std::make_shared<TypeDeserializerImpl>();
            m_tuple_deserializer.reset();
            break;
        default:
            throw std::invalid_argument("Unknown record type " + std::to_string(m_record_type));
        }
    }

    DeserializationResult               get_next_record(std::shared_ptr<IOReadableWritable> target) {
        switch (m_record_type) {
        case RECORD_TYPE::NORMAL:
            return m_deserializer->get_next_record(target);
        case RECORD_TYPE::TUPLE:
            return m_tuple_deserializer->get_next_record(target);
        default:
            throw std::invalid_argument("Unknown record type " + std::to_string(m_record_type));
        }
    }
    void                                set_next_buffer(std::shared_ptr<BufferBase> buffer) {
        switch (m_record_type)
        {
        case RECORD_TYPE::NORMAL:
            m_deserializer->set_next_buffer(buffer);
            break;
        case RECORD_TYPE::TUPLE:
            m_tuple_deserializer->set_next_buffer(buffer);
            break;
        default:
            throw std::invalid_argument("Unknown record type " + std::to_string(m_record_type));
        }
        
    }
};
