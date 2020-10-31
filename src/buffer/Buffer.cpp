#include "Buffer.hpp"
int Buffer::m_id_counter = 0;
std::shared_ptr<spdlog::logger>  Buffer::m_logger = LoggerFactory::get_logger("Buffer");

/**
 * Create a new buffer, allocate new memory.
 */
Buffer::Buffer(int capacity) {
    //TODO: OOM problem check
    m_data      = new unsigned char[capacity];
    m_size      = capacity;
    m_buffer_id = get_next_id();
    m_buffer_pool_manager = nullptr;

    // set logger
    spdlog::set_level(Constant::SPDLOG_LEVEL);
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
}

Buffer::Buffer(int capacity, std::shared_ptr<BufferPoolManager> buffer_pool_mananger): Buffer(capacity) {
    this->m_buffer_pool_manager = buffer_pool_mananger;
}

int Buffer::get(unsigned char* buf, int index) {
    if (index >= m_size) {
        // std::cerr << "index [" << index << "] is out of the range [" << m_size << "]" << std::endl;
        return -1;
    }
    (*buf) = m_data[index];
    return 1;
}

void Buffer::put(int index, const unsigned char value) {
    if (index >= m_size) {
        std::string error_info = "index [" + std::to_string(index) + "] is out of the range [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    m_data[index] = value;
}

void Buffer::put(int index, const unsigned char* const value, int length) {
    if (length == 0) return;
    if (index + length > m_size) {
        std::string error_info = "index + length [" + std::to_string(index + length) + "] is out of the range [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    memcpy(m_data + index, value, length);
}

// BufferBase* Buffer::read_only_slice(int offset, int length) {
//     if (offset >= m_size || offset + length > m_size) {
//         std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
//          "], capacity [" + std::to_string(m_size) + "]";
//         throw std::invalid_argument(error_info);
//     }
//     return new ReadOnlySlidedBuffer(this, offset, length);
// }

std::shared_ptr<BufferBase> Buffer::read_only_slice(int offset, int length) {
    if (offset >= m_size || offset + length > m_size) {
        std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
         "], capacity [" + std::to_string(m_size) + "]";
        // std::cout << error_info << std::endl;
        SPDLOG_LOGGER_ERROR(m_logger, error_info);
        return nullptr;
    }
    return std::make_shared<ReadOnlySlidedBuffer>(this, offset, length);
}