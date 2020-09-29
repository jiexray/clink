#include "BufferConsumer.hpp"

std::shared_ptr<spdlog::logger> BufferConsumer::m_logger = LoggerFactory::get_logger("BufferConsumer");

bool BufferConsumer::is_data_available() {
    if (m_write_position_marker_ptr == nullptr) {
        return false;
    }
    return m_read_position_marker < *m_write_position_marker_ptr;
}

bool BufferConsumer::is_finished() {
    // The buffer consumer has finished reading all buffers from Buffer, and
    // set write_position_marker_ptr to null
    if (m_write_position_marker_ptr == nullptr) {
        return true;
    }

    // May have bug, the m_write_position_marker is moved by builder, however, the 
    // the consumer fail to read the content from it. The BufferConsumer is marked
    // as finished, the data between the write_position and read_position is left
    // unread forever! 
    // Check the position of read_marker, if not equal to write_marker, return 
    int cached_write_position = *m_write_position_marker_ptr; // must one read!
    if (m_read_position_marker != cached_write_position) {
        // something left to read, return immediately
        return false;
    }
    return cached_write_position == m_buffer->get_max_capacity();
}

int BufferConsumer::get_write_position() {
    if (m_write_position_marker_ptr == nullptr) {
        return -1;
    }
    return *m_write_position_marker_ptr;
}

BufferConsumer::BufferConsumer(Buffer* buffer, std::atomic_int* current_write_position_ptr, int current_read_position):
m_buffer(buffer), 
m_write_position_marker_ptr(current_write_position_ptr), 
m_read_position_marker(0){
    // setup logger
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);

    // register BufferConsumer to BufferPoolManager
    if (m_buffer->get_buffer_pool_manager() != nullptr)
        m_buffer->get_buffer_pool_manager()->register_buffer_consumer(m_buffer->get_buffer_id());
}

BufferConsumer::~BufferConsumer(){
    if (m_buffer->get_buffer_pool_manager() != nullptr)
        m_buffer->get_buffer_pool_manager()->unregister_buffer_consumer(m_buffer->get_buffer_id());
}

std::shared_ptr<BufferBase> BufferConsumer::build() {
    if (m_read_position_marker == *m_write_position_marker_ptr) {
        // current buffer is empty
        return nullptr;
    }
    /**
     * Note: we can only read writer position marker one time using atomic read.
     *       (1) store the previous read position;
     *       (2) get the target read postition
     *       (3) return the buffer slice between the old and target read positions.
     */
    int old_read_position = m_read_position_marker;
    m_read_position_marker = m_write_position_marker_ptr->load();
    std::shared_ptr<BufferBase> bufferSlice = m_buffer->read_only_slice(old_read_position, m_read_position_marker - old_read_position);

    // check finish and clear write_position_marker
    if (m_read_position_marker == m_buffer->get_max_capacity()) {
        delete m_write_position_marker_ptr;
        m_write_position_marker_ptr = nullptr;
    }

    // // DEBUG
    // std::ostringstream oss;
    // oss << "slice dump, size" << bufferSlice->get_max_capacity() << " value: ";
    // for(int i = 0; i < bufferSlice->get_max_capacity(); i++) {
    //     char c;
    //     bufferSlice->get(&c, i);
    //     oss << (int)c << " ";
    // }
    // SPDLOG_LOGGER_INFO(m_logger, oss.str());
    return bufferSlice;
}