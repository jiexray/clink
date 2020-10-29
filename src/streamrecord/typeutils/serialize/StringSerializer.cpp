#include "StringSerializer.hpp"
/**
 * Note: the user should maintaince the write state! through m_data_remaining. 
 * This class is not thread-safe, and thus each ResultPartition needs to create a self-use instance for copy StreamRecord
 * to BufferBuilder (provided by outside).
 */
StreamRecordAppendResult StringSerializer::serialize(std::shared_ptr<std::string> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record){
    int value_size = record->size();
    if (is_new_record) {
        m_data_remaining = value_size + 2;
        unsigned char* length_buf = new unsigned char[2];
        SerializeUtils::serialize_short(length_buf, value_size);
        int data_length_write = buffer_builder->append(length_buf, 0, 2, false);
        delete[] length_buf;
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
            delete[] length_buf;
            // the length of record is not totally written, partially write
            m_data_remaining -= data_length_write;
            assert(data_length_write == 2);
        } else if (left_data_length == 1) {
            unsigned char* length_buf = new unsigned char[2];
            SerializeUtils::serialize_short(length_buf, value_size);
            // start write from offset 1, write length 1
            int data_length_write = buffer_builder->append(length_buf, 1, 1, false);
            delete[] length_buf;
            // the length of record is not totally written, partially write
            m_data_remaining -= data_length_write;
            assert(data_length_write == 1);
        } else {
            throw std::runtime_error("unwritten data_length is illegal: " + std::to_string(m_data_remaining - value_size));
        }
    }

    m_data_remaining -= buffer_builder->append((unsigned char*)record->c_str(), value_size - m_data_remaining, m_data_remaining);
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