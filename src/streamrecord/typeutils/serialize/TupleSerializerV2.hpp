/**
 * Serializer util for tuple types
 */
#pragma once 

#include "Tuple2.hpp"
#include "TypeSerializer.hpp"
#include "TypeSerializerUtil.hpp"
#include <assert.h>

// class TypeSerializerDelegate;

template <class T> class TupleSerializerV2;

template <class T0, class T1>
class TupleSerializerV2<Tuple2<T0, T1>> : public TypeSerializer<Tuple2<T0, T1>>
{
private:
    int                                         m_arity;
    bool*                                       m_is_finished;
    bool*                                       m_is_new_record;
    std::shared_ptr<TypeSerializerBase>*        m_field_serializers;
    int                                         m_data_remaining;
    unsigned char*                              m_length_buf;
    unsigned char*                              m_tuple_buf;
    int                                         m_tuple_buf_write_pos;
    int                                         m_value_size;
public:

    TupleSerializerV2() {
        m_arity = 2;
        m_is_finished = new bool[m_arity + 1];
        m_is_new_record = new bool[m_arity + 1];
        m_field_serializers = new std::shared_ptr<TypeSerializerBase>[m_arity + 1];

        m_field_serializers[0] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T0));
        m_field_serializers[1] = TypeSerializerUtil::create_type_basic_type_serializer(typeid(T1));

        m_length_buf = new unsigned char[2];

        m_tuple_buf = new unsigned char[65536];
    }

    ~TupleSerializerV2() {
        delete[] m_is_finished;
        delete[] m_is_new_record;
        delete[] m_field_serializers;
        delete[] m_length_buf;
        delete[] m_tuple_buf;
    }

    StreamRecordAppendResult    serialize(std::shared_ptr<Tuple2<T0, T1>> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

template <class T0, class T1>
inline StreamRecordAppendResult TupleSerializerV2<Tuple2<T0, T1>>::serialize(std::shared_ptr<Tuple2<T0, T1>> tuple, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) {
    

    if (is_new_record) {
        m_value_size = tuple->get_buf_size() + 2;
        m_data_remaining = m_value_size;

        if (m_data_remaining > 65536) {
            throw std::runtime_error("a tuple size " + tuple->to_string() + " cannot over 65536 bytes");
        }
        assert(tuple->get_arity() == 2);

        int cur_write_pos = 0;

        // write tuple length
        SerializeUtils::serialize_short(m_tuple_buf + cur_write_pos, m_value_size - 2);
        cur_write_pos += 2;

        // write first val length
        int f0_size = tuple->get_value_buf_size(Int2Type<0>(), Type2Type<T0>());
        SerializeUtils::serialize_short(m_tuple_buf + cur_write_pos, f0_size);
        cur_write_pos += 2;
        // write first val
        memcpy(m_tuple_buf + cur_write_pos, tuple->get_value_char_ptr(Int2Type<0>(), Type2Type<T0>()), f0_size);
        cur_write_pos += f0_size;

        // write second val length
        int f1_size = tuple->get_value_buf_size(Int2Type<1>(), Type2Type<T1>());
        SerializeUtils::serialize_short(m_tuple_buf + cur_write_pos, f1_size);
        cur_write_pos += 2;
        // write second val length
        memcpy(m_tuple_buf + cur_write_pos, tuple->get_value_char_ptr(Int2Type<1>(), Type2Type<T1>()), f1_size);
        cur_write_pos += f1_size;

        assert(cur_write_pos == m_data_remaining);
    }

    m_data_remaining -= buffer_builder->append(m_tuple_buf, m_value_size - m_data_remaining, m_data_remaining);

    if (m_data_remaining == 0) {
        if (buffer_builder->is_full()) {
            return FULL_RECORD_BUFFER_FULL;
        } else {
            return FULL_RECORD;
        }
    } else {
        return PARTITAL_RECORD_BUFFER_FULL;
    }
}