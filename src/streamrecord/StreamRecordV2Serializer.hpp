/**
 * Serializer for StreamRecordV2.
 */
#pragma once
#include "StreamRecordV2.hpp"
#include "BufferBuilder.hpp"

template <class T>
class StreamRecordV2Serializer;

template <class T>
class StreamRecordV2Serializer {
private:
    int                                     m_data_remaining;
    int                                     m_value_size;
public:
    StreamRecordAppendResult                serialize(const T& record, 
                                                        std::shared_ptr<BufferBuilder> buffer_builder,
                                                        bool is_new_record){
        if (is_new_record) {
            // do NOT write stream record size
            m_value_size = sizeof(T);
            m_data_remaining = m_value_size;

            if (m_data_remaining > 65536) {
                throw std::runtime_error("a record size: " + std::to_string(m_data_remaining) + " cannot over 65536 bytes");
            } 
        }

        m_data_remaining -= buffer_builder->append((const unsigned char*)(&record), m_value_size - m_data_remaining, m_data_remaining);

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
};