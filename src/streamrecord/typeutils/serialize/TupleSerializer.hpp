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

template <class T0, class T1>
class TupleSerializer<Tuple2<T0, T1>> : public TypeSerializer<Tuple2<T0, T1>>
{
private:
    int                                         m_arity;
    bool*                                       m_is_finished;
    bool*                                       m_is_new_record;
    std::shared_ptr<TypeSerializerBase>*        m_field_serializers;
    int                                         m_data_remaining;
    unsigned char*                              m_length_buf;
public:

    TupleSerializer() {
        m_arity = 2;
        m_is_finished = new bool[m_arity + 1];
        m_is_new_record = new bool[m_arity + 1];
        m_field_serializers = new std::shared_ptr<TypeSerializerBase>[m_arity + 1];

        m_field_serializers[0] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T0));
        m_field_serializers[1] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T1));

        m_length_buf = new unsigned char[2];
    }

    ~TupleSerializer() {
        delete[] m_is_finished;
        delete[] m_is_new_record;
        delete[] m_field_serializers;
        delete[] m_length_buf;
    }

    StreamRecordAppendResult    serialize(std::shared_ptr<Tuple2<T0, T1>> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

template <class T0, class T1>
inline StreamRecordAppendResult TupleSerializer<Tuple2<T0, T1>>::serialize(std::shared_ptr<Tuple2<T0, T1>> tuple, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) {
    int value_size = tuple->get_buf_size();

    if (is_new_record) {
        m_data_remaining = value_size + 2;
        assert(tuple->get_arity() == 2);

        for (int i = 0; i < m_arity; i++) {
            m_is_finished[i] = false;
            m_is_new_record[i] = true;
        }

        SerializeUtils::serialize_short(m_length_buf, value_size);
        int data_length_write = buffer_builder->append(m_length_buf, 0, 2, false);

        // the length of record is not totally written, partially write
        m_data_remaining -= data_length_write;
        if (data_length_write < 2) {
            return PARTITAL_RECORD_BUFFER_FULL;
        }
    }

    // check if data_length is completely written
    if (m_data_remaining > value_size) {
        // have unwritten data_length
        int left_data_length = m_data_remaining - value_size;
        if (left_data_length == 2) {
            unsigned char* length_buf = new unsigned char[2];
            SerializeUtils::serialize_short(length_buf, value_size);
            int data_length_write = buffer_builder->append(length_buf, 0, 2, false);
            // the length of record is not totally written, partially write
            m_data_remaining -= data_length_write;
            assert(data_length_write == 2);
        } else if (left_data_length == 1) {
            unsigned char* length_buf = new unsigned char[2];
            SerializeUtils::serialize_short(length_buf, value_size);
            // start write from offset 1, write length 1
            int data_length_write = buffer_builder->append(length_buf, 1, 1, false);
            // the length of record is not totally written, partially write
            m_data_remaining -= data_length_write;
            assert(data_length_write == 1);
        } else {
            throw std::runtime_error("unwritten data_length is illegal: " + std::to_string(m_data_remaining - value_size));
        }
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
         
        m_is_new_record[i] = false;

        // check whether data in the field is all written into buffer
        if (partial_result != StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL) {
            // check if the field is the final field?
            if (i == m_arity - 1) {
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