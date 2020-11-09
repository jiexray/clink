/**
 * Deserializer for StreamRecordV2.
 */
#pragma once

#include "StreamRecordV2.hpp"
#include "BufferBase.hpp"
#include "LoggerFactory.hpp"
#include <deque>
#include <memory>

template <class T>
class StreamRecordV2Deserializer
{
private:
    std::deque<std::shared_ptr<BufferBase>>         m_last_buffers;
    int                                             m_record_size;

    int                                             m_position;
    int                                             m_remaining;
    static std::shared_ptr<spdlog::logger>          m_logger;

    T*      m_record;
    T*      m_record_backup;
    bool    m_to_commit;
public:
    StreamRecordV2Deserializer(){
        m_record_size = -1;
        m_position = -1;
        m_remaining = 0;

        m_record_backup = new T();
    }

    ~StreamRecordV2Deserializer() {
        delete m_record_backup;
    }

    void                                            set_next_buffer(std::shared_ptr<BufferBase> buffer){
        if ((int)m_last_buffers.size() == 0) {
            // empty buffer, initialize the offset
            m_position = 0;
        }
        m_last_buffers.push_back(buffer);
        m_remaining += buffer->get_max_capacity();
    }

    DeserializationResult                           get_next_record() {
        if (m_record_size == -1) {
            if (m_position == -1) {
                throw std::runtime_error("cannot deserialize record, when buffers in empty.");
            }

            if (m_remaining < 2) {
                // SPDLOG_LOGGER_ERROR(m_logger, "Insufficient length to read a short! remaining: {}", m_remaining);
                return DeserializationResult::PARTIAL_RECORD;
            }
            m_record_size = sizeof(T);
        } 

        if (m_record_size <= m_remaining) {
            read();

            m_record_size = -1;

            return m_remaining == 0 ? DeserializationResult::LAST_RECORD_FROM_BUFFER : DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER;
        }
        return DeserializationResult::PARTIAL_RECORD;
    }

    void read()  {
        // if the buffer is in a full buffer block, directly return m_record
        m_record = nullptr;
        m_to_commit = true;
        read_record_no_copy(&m_record);

        // else copy the buffer to record
        if (m_record == nullptr) {
            m_to_commit = false;
            read_record(m_record_backup);
            m_record = m_record_backup;
        }
    }

    T* get_instance() {
        return m_record;
    }

    /**
      Read record (type T) with no copy.
     */
    void                    read_record_no_copy(T** rec) {
        int left_bytes_in_front_buffer = m_last_buffers.front()->get_max_capacity() - m_position;
        int left_length = sizeof(T);

        if (left_bytes_in_front_buffer >= left_length) {
            m_last_buffers.front()->get_nocopy((unsigned char**)(rec), m_position, left_length);
            m_position += left_length;
            m_remaining -= left_length;
        } else {
            *rec = nullptr;
        }
    }

    /**
      Commit a none-copy record read.
     */
    void                    read_commit() {
        if (m_to_commit) {
            evict_used_buffer();
        }
    } 

    void                    read_record(T* rec) {
        int left_bytes_in_front_buffer = m_last_buffers.front()->get_max_capacity() - m_position;
        int left_length = sizeof(T);

        if (left_bytes_in_front_buffer >= left_length) {
            m_last_buffers.front()->get((unsigned char*)rec, m_position, left_length);
            m_position += left_length;
            m_remaining -= left_length;
            evict_used_buffer();
        } else {
            int to_write = left_bytes_in_front_buffer;

            while (left_length != 0) {
                m_last_buffers.front()->get(((unsigned char*)rec) + (sizeof(T) - left_length), m_position, to_write);
                left_length -= to_write;
                m_position += to_write;
                m_remaining -= to_write;
                evict_used_buffer();

                left_bytes_in_front_buffer = m_last_buffers.front()->get_max_capacity() - m_position;
                
                if (left_bytes_in_front_buffer >= left_length) {
                    to_write = left_length;
                } else {
                    to_write = left_bytes_in_front_buffer;
                }
            }
            
        }
    }

    /**
      Evict already used buffers.
     */
    void evict_used_buffer() {
        if (m_last_buffers.empty()) {
            SPDLOG_LOGGER_DEBUG(m_logger, "evict_used_buffer(): useless buffer evict");
            return;
        }
        std::shared_ptr<BufferBase> first_buf = m_last_buffers.front();
        int buf_size = first_buf->get_max_capacity();
        // only one byte left is also a completed buf, nothing to read

        if (m_position == buf_size) {
            m_last_buffers.pop_front();
            if ((int) m_last_buffers.size() == 0) {
                // nothing left in deserializer
                m_position = -1;
            } else {
                m_position = 0;
            }

            // assert the use_count of the first buffer, it will be destroy sooner
            if (first_buf.use_count() != 1) {
                SPDLOG_LOGGER_ERROR(m_logger, "Ref count of a buffer slice in Buffer {} is not zero, when finish reading", 
                                        first_buf->get_buffer_id());
                std::cout << "Ref count of a buffer slice in Buffer " << first_buf->get_buffer_id() <<" is not zero, when finish reading" << std::endl;
            }
        } 
    }

};

template <class T>
std::shared_ptr<spdlog::logger> StreamRecordV2Deserializer<T>::m_logger = LoggerFactory::get_logger("StreamRecordV2Deserializer");
