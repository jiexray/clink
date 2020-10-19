#include "BufferBuilder.hpp"
std::shared_ptr<spdlog::logger> BufferBuilder:: m_logger = LoggerFactory::get_logger("BufferBuilder");

bool BufferBuilder::is_full() {
    // sychronize function in write part
    return m_buffer->get_max_capacity() == m_cached_write_postition;
}

BufferBuilder::BufferBuilder(Buffer* buffer):
m_buffer(buffer) {
    m_cached_write_postition = 0;
    m_write_position_marker_ptr = new std::atomic_int{0};

    // setup logger
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);    
}

BufferBuilder::~BufferBuilder() {}

int BufferBuilder::append(const char* const source, int offset, int length) {
    // sychronize function in write part
    int buffer_capacity = m_buffer->get_max_capacity();
    // NOTE: this get and set of writer_marker is ok, for the writer marker is only modified in this file.
    int available = buffer_capacity - m_cached_write_postition;
    int to_copy = std::min(available, length);

    for (int i = 0; i < to_copy; i++) {
        m_buffer->put(m_cached_write_postition++, source[offset + i]);
        // all things are approprite done, builder can die

        // update m_write_position_marker_ptr, a notify to BufferConsumer
        // here m_write_position_marker_ptr can never be null, there are thing left for BufferConsumer to read
        (*m_write_position_marker_ptr)++;
    }
    return to_copy;
}

int BufferBuilder::append(const char* const source, int offset, int length, bool must_complete){
    // sychronize function in write part
    if (!must_complete) {
        return append(source, offset, length);
    } else {
        int buffer_capacity = m_buffer->get_max_capacity();
        int available = buffer_capacity - m_cached_write_postition;
        if (available < length) {
            // force to fill the unfinished buffer with fake chars
            for (int i = 0; i < available; i++) {
                // same to append(const char* const, int, int)
                m_buffer->put(m_cached_write_postition++, (char)0);

                (*m_write_position_marker_ptr)++;
            }
            return 0;
        } else {
            return append(source, offset, length);
        }
    }
}

std::shared_ptr<BufferConsumer> BufferBuilder::create_buffer_consumer() {
    return std::make_shared<BufferConsumer>(m_buffer, m_write_position_marker_ptr, 0);
}

int BufferBuilder::finish() {
    return m_buffer->get_max_capacity();
}