#include "ReadOnlySlidedBuffer.hpp"
std::shared_ptr<spdlog::logger> ReadOnlySlidedBuffer::m_logger = LoggerFactory::get_logger("ReadOnlySlidedBuffer");

ReadOnlySlidedBuffer::ReadOnlySlidedBuffer(BufferBase* buffer, int offset, int length):
parent_buffer((Buffer*)buffer),
m_buffer_offset(offset),
m_size(length){
    // setup logger
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

int ReadOnlySlidedBuffer::get_buffer_id() {
    return parent_buffer->get_buffer_id();
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
    return parent_buffer->read_only_slice(m_buffer_offset + offset, length);
}

int ReadOnlySlidedBuffer::get(unsigned char* buf, int index) {
    if (index >= m_size) {
        std::cerr << "ReadOnlySlidedBuffer::get() index [" << index << "] is out of the range [" << m_size << "]" << std::endl;
        return -1; 
    }
    return parent_buffer->get(buf, m_buffer_offset + index);
}

int ReadOnlySlidedBuffer::get(unsigned char* buf, int index, int length) {
    if  (index + length > m_size) {
        std::string error_info = "ReadOnlySlidedBuffer::get() index + length [" + std::to_string(index + length) + "] is out of the range [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    return parent_buffer->get(buf, m_buffer_offset + index, length);
} 

void ReadOnlySlidedBuffer::get_nocopy(unsigned char** buf, int index, int length) {
    if  (index + length > m_size) {
        std::string error_info = "ReadOnlySlidedBuffer::get() index + length [" + std::to_string(index + length) + "] is out of the range [" + std::to_string(m_size) + "]";
        throw std::invalid_argument(error_info);
    }
    parent_buffer->get_nocopy(buf, m_buffer_offset + index, length);
}

void ReadOnlySlidedBuffer::release(){
    if (parent_buffer->get_buffer_pool_manager() != nullptr){
        parent_buffer->get_buffer_pool_manager()->unregister_buffer_slice(parent_buffer->get_buffer_id());
    }
}