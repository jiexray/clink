/**
 * Serializer util for tuple types
 */
#pragma once 

#include "Tuple.hpp"
#include "Tuple2.hpp"
#include "TypeSerializer.hpp"
#include "TypeSerializerUtil.hpp"
#include "assert.h"

// class TypeSerializerDelegate;

template <class T> class TupleSerializer;

template <class T1, class T2>
class TupleSerializer<Tuple2<T1, T2>> : public TypeSerializer<Tuple2<T1, T2>>
{
private:
    int                                         m_arity;
    bool*                                       m_is_finished;
    bool*                                       m_is_new_record;
    std::shared_ptr<TypeSerializerBase>*        m_field_serializers;
public:

    StreamRecordAppendResult    serialize(std::shared_ptr<Tuple2<T1, T2>> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

template <class T0, class T1>
inline StreamRecordAppendResult TupleSerializer<Tuple2<T0, T1>>::serialize(std::shared_ptr<Tuple2<T0, T1>> tuple, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) {
    if (is_new_record) {
        int total_tuple_size = tuple->get_buf_size();
        char* length_buf = new char[2];
        SerializeUtils::serialize_short(length_buf, total_tuple_size);
        int data_length_write = buffer_builder->append(length_buf, 0, 2, true);
        delete length_buf;
        if(data_length_write == 0) {
            return NONE_RECORD;
        }

        m_arity = tuple->get_arity();
        assert(m_arity == 2);
        m_is_finished = new bool[m_arity + 1];
        m_is_new_record = new bool[m_arity + 1];
        m_field_serializers = new std::shared_ptr<TypeSerializerBase>[m_arity + 1];

        for (int i = 0; i < m_arity; i++) {
            m_is_finished[i] = false;
            m_is_new_record[i] = true;
            
            // m_field_serializers[i] = std::make_shared<TypeSerializerDelegate>(tuple->get_field(i));
        }
        m_field_serializers[0] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T0));
        m_field_serializers[1] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T1));
    }

    for (int i = 0; i < m_arity; i++) {
        if (m_is_finished[i]) {
            continue;
        }

        StreamRecordAppendResult partial_result;
        switch (i) {
        case 0:
            partial_result = std::dynamic_pointer_cast<TypeSerializer<T0>>(m_field_serializers[i])->serialize(
                                                        std::static_pointer_cast<T0>(tuple->get_value(i)), buffer_builder, m_is_new_record[i]);
            break;
        case 1:
            partial_result = std::dynamic_pointer_cast<TypeSerializer<T1>>(m_field_serializers[i])->serialize(
                                                        std::static_pointer_cast<T1>(tuple->get_value(i)), buffer_builder, m_is_new_record[i]);
            break;
        }
         

        // check whether the length of data is written into buffer
        if (m_is_new_record[i] && partial_result == StreamRecordAppendResult::NONE_RECORD){
            // the data length of this field is not written, need new BufferBuilder
            return StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL;
        }
        m_is_new_record[i] = false;

        // check whether data in the field is all written into buffer
        if (partial_result != StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL) {
            // check if the field is the final field?
            if (i == m_arity - 1) {
                // finish all serialization of an tuple, free buffer
                delete[] m_is_finished;
                delete[] m_is_new_record;
                for (int i = 0; i < m_arity; i++) {
                    m_field_serializers[i].reset();
                }
                delete[] m_field_serializers;

                m_is_finished = nullptr;
                m_is_new_record = nullptr;
                m_field_serializers = nullptr;

                return partial_result;
            } else {
                m_is_finished[i] = true;
                // continue to next buffer
                continue;
            }
        }

        // partial_result is PARTITAL_RECORD_BUFFER_FULL, need more buffer, one record is not fully written
        assert(partial_result == StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL);

        return StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL;
    }
}