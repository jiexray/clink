/**
 * A serializer for int type.
 */
#pragma once
#include "TypeSerializer.hpp"
#include <iostream>

class IntSerializer : public TypeSerializer<int>
{
private:
    int         m_data_remaining;
    char*       m_data_in_char;
public:
    StreamRecordAppendResult        serialize(std::shared_ptr<int> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

// StreamRecordAppendResult IntSerializer::serialize(std::shared_ptr<int> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record){
//     // std::cout << "double record value: " << *record.get() << ", double size: " << sizeof(double) << std::endl;
//     if (is_new_record) {
//         m_data_remaining = sizeof(int);
//         m_data_in_char = (char*) record.get();
//         char* length_buf = new char[2];
//         SerializeUtils::serialize_short(length_buf, 4);
//         int data_length_write = buffer_builder->append(length_buf, 0, 2, true);
//         delete length_buf;
//         // the length of record is not totally written, skip the buffer
//         if (data_length_write == 0) {
//             return NONE_RECORD;
//         }
//     }
//     int value_size = sizeof(int);
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

