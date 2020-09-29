/**
 * An interface for serializing everything to char*, which will be stored in Buffer.
 */
#pragma once
#include <memory>
#include <string>
#include "BufferBuilder.hpp"
#include "StreamRecord.hpp"
#include "SerializeUtils.hpp"
#include "assert.h"

class TypeSerializerBase {
public:
    virtual ~TypeSerializerBase() = default;
};

template <class T>
class TypeSerializer : public TypeSerializerBase
{
public:
    // copy the record into buffer builder
    virtual StreamRecordAppendResult    serialize(std::shared_ptr<T> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) = 0;
};



