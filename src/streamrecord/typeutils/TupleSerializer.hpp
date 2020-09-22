/**
 * Serializer util for tuple types
 */
#pragma once 

#include "Tuple.hpp"
#include "Tuple2.hpp"
#include "TypeSerializerDelegate.hpp"
#include "assert.h"

class TypeSerializerDelegate;

class TupleSerializer : public TypeSerializer<Tuple>
{
private:
    int                                         m_arity;
    bool*                                       m_is_finished;
    bool*                                       m_is_new_record;
    std::shared_ptr<TypeSerializerDelegate>*    m_field_serializers;
public:

    StreamRecordAppendResult    serialize(std::shared_ptr<Tuple> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};