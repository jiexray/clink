#include "TypeDeserializerImpl.hpp"
std::shared_ptr<spdlog::logger> TypeDeserializerImpl::m_logger = LoggerFactory::get_logger("TypeDeserializerImpl");


void TypeDeserializerImpl::set_next_buffer(std::shared_ptr<BufferBase> buffer) {
    if ((int)m_last_buffers.size() == 0) {
        // empty buffer, initialize the offset
        m_position = 0;
    }
    m_last_buffers.push_back(buffer);
    m_remaining += buffer->get_max_capacity();
}

DeserializationResult TypeDeserializerImpl::get_next_record(std::shared_ptr<IOReadableWritable> target) {
    if (m_record_size == -1) {
        if (m_position == -1) {
            throw new std::runtime_error("cannot deserialize record, when buffers in empty.");
        }

        if (m_remaining < 2) {
            // SPDLOG_LOGGER_ERROR(m_logger, "Insufficient length to read a short! remaining: {}", m_remaining);
            return DeserializationResult::PARTIAL_RECORD;
        }
        m_record_size = read_short();
        // check the end of one buffer
        // BUG: we do not need to check the end of one buffer. For, if currently there is only 
        //      one byte in the buffer, we can just read it for the data can be splitted.
        //      We just need to check the one buffer if we finish one read, for there cannot locate
        //      a data-len in short (2 bytes).
    } 

    if (m_record_size <= m_remaining) {
        return read_into(target);
    }
    return DeserializationResult::PARTIAL_RECORD;
}

DeserializationResult TypeDeserializerImpl::read_into(std::shared_ptr<IOReadableWritable> target) {
    target->read(this);

    // finish reading, re-initialize m_record_size
    m_record_size = -1;


    return m_remaining == 0 ? DeserializationResult::LAST_RECORD_FROM_BUFFER : DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER;
}


int TypeDeserializerImpl::read_int() {
    unsigned char* buf = new unsigned char[4];
    // TODO: use more efficient buffer copy
    for (int i = 0; i < 4; i++) {
        int ret = m_last_buffers.front()->get(buf + i, m_position++);
        if (ret == -1) {
            throw new std::runtime_error("read error, reach end, bug in evict used buffer()");
        }
        m_remaining--;
        evict_used_buffer(false);
    }
    int v = SerializeUtils::deserialize_int(buf);

    // free buf
    delete buf;
    buf = nullptr;

    return v;
}

int TypeDeserializerImpl::read_short() {
    if (m_last_buffers.empty()) {
        throw std::runtime_error("read error, empty buffer lists");
    }
    unsigned char* buf = new unsigned char[2];

    for (int i = 0; i < 2; i++) {
        // SPDLOG_LOGGER_INFO(m_logger, dump_state());
        int ret = m_last_buffers.front()->get(buf + i, m_position++);
        if (ret == -1) {
            throw new std::runtime_error("read error, reach end, bug in evict used buffer()");
        }
        m_remaining--;
        evict_used_buffer(false);
    }
    int v = SerializeUtils::deserialize_short(buf);

    // free buf
    delete buf;
    buf = nullptr;

    return v;
}

int TypeDeserializerImpl::read_byte() {
    unsigned char* buf = new unsigned char[1];
    int ret = m_last_buffers.front()->get(buf, m_position++);
    if (ret == -1) {
        throw new std::runtime_error("error, no buffer to read for read_byte()");
    }
    m_remaining--;
    evict_used_buffer(false);
    int v = (int)buf[0];

    // free buf
    delete buf;
    buf = nullptr;

    return v;
}

int TypeDeserializerImpl::read_unsigned_byte() {
    return read_byte() & 0xff;
}

double TypeDeserializerImpl::read_double() {
    unsigned char* buf = new unsigned char[8];

    for (int i = 0; i < 8; i++) {
        int ret = m_last_buffers.front()->get(buf + i, m_position++);
        if (ret == -1) {
            throw new std::runtime_error("read error, reach end, bug in evict used buffer()");
        }
        m_remaining--;
        evict_used_buffer(false);
    }
    double v = SerializeUtils::deserialize_double(buf);

    // free buf
    delete buf;
    buf = nullptr;
    
    return v;
}

/**
 * Check whether the m_position reaches the end of the top buffer in m_last_buffers.
 * If so, evict it from the queue.
 * 
 * NOTE: If there is only one byte left, the buffer is finished as well. For it can not store a int for record length.
 * 
 * TODO: Recycle the buffer if needed.
 */
void TypeDeserializerImpl::evict_used_buffer(bool is_finish_read) {
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


