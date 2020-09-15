#include "BufferBuilder.hpp"
std::shared_ptr<spdlog::logger> BufferBuilder:: m_logger = spdlog::get("BufferBuilder") == nullptr?
                                                            spdlog::basic_logger_mt("BufferBuilder", Constant::get_log_file_name()):
                                                            spdlog::get("BufferBuilder");

bool BufferBuilder::is_full() {
    return m_buffer->get_max_capacity() == *m_write_position_marker_ptr;
}

BufferBuilder::BufferBuilder(Buffer* buffer):
m_buffer(buffer) {
    m_write_position_marker_ptr = new std::atomic_int{0};

    // setup logger
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);    
}

BufferBuilder::~BufferBuilder() {}

int BufferBuilder::append(const char* const source, int offset, int length) {
    int buffer_capacity = m_buffer->get_max_capacity();
    // NOTE: this get and set of writer_marker is ok, for the writer marker is only modified in this file.
    int available = buffer_capacity - *m_write_position_marker_ptr;
    int to_copy = std::min(available, length);

    for (int i = 0; i < to_copy; i++) {
        m_buffer->put((*m_write_position_marker_ptr)++, source[offset + i]);
    }
    return to_copy;
}

int BufferBuilder::append(const char* const source, int offset, int length, bool must_complete){
    if (!must_complete) {
        return append(source, offset, length);
    } else {
        int buffer_capacity = m_buffer->get_max_capacity();
        int available = buffer_capacity - *m_write_position_marker_ptr;
        if (available < length) {
            // force to fill the unfinished buffer with fake chars
            for (int i = 0; i < available; i++) {
                m_buffer->put((*m_write_position_marker_ptr)++, (char)0);
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

void BufferBuilder::recycle_buffer_consumer() {
    // TODO: recycle buffer consumer before free buffer builder

}