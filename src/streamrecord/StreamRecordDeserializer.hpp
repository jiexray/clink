/**
 * Interface for turning sequences of memory segments into record.
 */
#pragma once
#include "../../buffer/BufferBase.hpp"
#include "typeutils/TypeDeserializer.hpp"
#include "types/IOReadableWritable.hpp"
#include "StreamRecord.hpp"
#include <memory>

class StreamRecordDeserializer
{
private:
    std::shared_ptr<TypeDeserializer>   m_deserializer;
public:
    StreamRecordDeserializer() {m_deserializer = std::make_shared<TypeDeserializer>();}

    DeserializationResult               get_next_record(std::shared_ptr<IOReadableWritable> target) {return m_deserializer->get_next_record(target);}
    void                                set_next_buffer(BufferBase* buffer) {m_deserializer->set_next_buffer(buffer);}
};
