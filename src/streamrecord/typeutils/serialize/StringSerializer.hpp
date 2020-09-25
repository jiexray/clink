/**
 * A serializer for string type.
 */
#pragma once
#include <string>
#include "TypeSerializer.hpp"

class StringSerializer : public TypeSerializer<std::string>
{
private:
        int                             m_data_remaining;
public:
        StreamRecordAppendResult        serialize(std::shared_ptr<std::string> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record);
};

// /**
//  * Note: the user should maintaince the write state! through m_data_remaining. 
//  * This class is not thread-safe, and thus each ResultPartition needs to create a self-use instance for copy StreamRecord
//  * to BufferBuilder (provided by outside).
//  */
// inline StreamRecordAppendResult StringSerializer::serialize(std::shared_ptr<std::string> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record){
//     if (is_new_record) {
//         m_data_remaining = record->size();
//         char* length_buf = new char[2];
//         SerializeUtils::serialize_short(length_buf, m_data_remaining);
//         int data_length_write = buffer_builder->append(length_buf, 0, 2, true);
//         delete length_buf;
//         // the length of record is not totally written, skip the buffer
//         if (data_length_write == 0) {
//             return NONE_RECORD;
//         }
//     }
//     int value_size = record->size();
//     m_data_remaining -= buffer_builder->append(record->c_str(), value_size - m_data_remaining, m_data_remaining);
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

