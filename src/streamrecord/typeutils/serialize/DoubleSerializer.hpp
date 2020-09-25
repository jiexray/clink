/**
 * A serializer for double type.
 */
#pragma once
#include "TypeSerializer.hpp"
#include <iostream>

class DoubleSerializer : public TypeSerializer<double>
{
private:
    int         m_data_remaining;
    char*       m_data_in_char;
public:
    StreamRecordAppendResult        serialize(std::shared_ptr<double> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

// inline StreamRecordAppendResult DoubleSerializer::serialize(std::shared_ptr<double> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record){
//     // std::cout << "double record value: " << *record.get() << ", double size: " << sizeof(double) << std::endl;
//     if (is_new_record) {
//         m_data_remaining = sizeof(double);
//         m_data_in_char = (char*) record.get();
//         char* length_buf = new char[2];
//         SerializeUtils::serialize_short(length_buf, 8);
//         int data_length_write = buffer_builder->append(length_buf, 0, 2, true);
//         delete length_buf;
//         // the length of record is not totally written, skip the buffer
//         if (data_length_write == 0) {
//             return NONE_RECORD;
//         }
//     }
//     int value_size = sizeof(double);
//     m_data_remaining -= buffer_builder->append(m_data_in_char, value_size - m_data_remaining, m_data_remaining);
//     if (m_data_remaining == 0) {
//         if (buffer_builder->is_full()) {
//             return FULL_RECORD_BUFFER_FULL;
//         } else {
//             return FULL_RECORD;
//         }
//     } else {
//         return PARTITAL_RECORD_BUFFER_FULL;
//     }
// }

