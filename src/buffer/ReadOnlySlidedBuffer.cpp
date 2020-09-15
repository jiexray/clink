#include "ReadOnlySlidedBuffer.hpp"
std::shared_ptr<spdlog::logger> ReadOnlySlidedBuffer::m_logger = spdlog::get("ReadOnlySlidedBuffer") == nullptr?
                                                                    spdlog::basic_logger_mt("ReadOnlySlidedBuffer", Constant::get_log_file_name()):
                                                                    spdlog::get("ReadOnlySlidedBuffer");


ReadOnlySlidedBuffer::ReadOnlySlidedBuffer(BufferBase* buffer, int offset, int length):
parent_buffer((Buffer*)buffer),
m_buffer_offset(offset),
m_size(length){
    // setup logger
    spdlog::set_level(Constant::SPDLOG_LEVEL);
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    SPDLOG_LOGGER_DEBUG(m_logger, "Create buffer slice of Buffer {} and register to BufferPoolManager", parent_buffer->get_buffer_id());
    if (parent_buffer->get_buffer_pool_manager() != nullptr){
        parent_buffer->get_buffer_pool_manager()->register_buffer_slice(parent_buffer->get_buffer_id());
    }
}

ReadOnlySlidedBuffer::~ReadOnlySlidedBuffer() {
    release();
}

int ReadOnlySlidedBuffer::get_buffer_offset() {
    return m_buffer_offset;
}

// BufferBase* ReadOnlySlidedBuffer::read_only_slice(int offset, int length) {
//     if (offset >= m_size || offset + length > m_size) {
//         std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
//          "], capacity [" + std::to_string(m_size) + "]";
//         throw std::invalid_argument(error_info);
//     }
//     parent_buffer->read_only_slice(m_buffer_offset + offset, length);
// }

std::shared_ptr<BufferBase> ReadOnlySlidedBuffer::read_only_slice(int offset, int length) {
    if (offset >= m_size || offset + length > m_size) {
        std::string error_info = "Slice is illegal offset [" + std::to_string(offset) + "], length [" + std::to_string(length) +
         "], capacity [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    parent_buffer->read_only_slice(m_buffer_offset + offset, length);
}

int ReadOnlySlidedBuffer::get(char* buf, int index) {
    if (index >= m_size) {
        std::cerr << "index [" << index << "] is out of the range [" << m_size << "]" << std::endl;
        return -1; 
    }
    parent_buffer->get(buf, m_buffer_offset + index);
}

void ReadOnlySlidedBuffer::release(){
    SPDLOG_LOGGER_DEBUG(m_logger, "ReadOnlySlidedBuffer released");
    if (parent_buffer->get_buffer_pool_manager() != nullptr){
        parent_buffer->get_buffer_pool_manager()->unregister_buffer_slice(parent_buffer->get_buffer_id());
    }
}