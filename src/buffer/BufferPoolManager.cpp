#include "BufferPoolManager.hpp"

std::shared_ptr<spdlog::logger> BufferPoolManager::m_logger = LoggerFactory::get_logger("BufferPoolManager");       

// -------------------
// BufferConsumer
// -------------------
void BufferPoolManager::register_buffer_consumer(int buffer_id){
    std::unique_lock<std::mutex> global_lck(m_buffer_manager_mtx);

    if (m_buffer_id_to_num_consumers.find(buffer_id) == m_buffer_id_to_num_consumers.end()) {
        throw std::runtime_error("BufferPoolManager Cannot find buffer_id " + std::to_string(buffer_id) + 
                                    ", it may not be registered");
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Register buffer consumer to Buffer {}", std::to_string(buffer_id));
    m_buffer_id_to_num_consumers[buffer_id] += 1;
}

void  BufferPoolManager::unregister_buffer_consumer(int buffer_id) {
    std::unique_lock<std::mutex> global_lck(m_buffer_manager_mtx);

    if (m_buffer_id_to_num_consumers.find(buffer_id) == m_buffer_id_to_num_consumers.end()) {
        throw std::runtime_error("BufferPoolManager Cannot find buffer_id " + std::to_string(buffer_id) + 
                                    ", it may not be registered");
    }
    int cur_num_consumers = m_buffer_id_to_num_consumers[buffer_id];
    if (cur_num_consumers == 0) {
        throw std::runtime_error("Number of register consumers is zero for buffer_id " + std::to_string(buffer_id));
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Unregister buffer consumer from Buffer {}", std::to_string(buffer_id));
    m_buffer_id_to_num_consumers[buffer_id] -= 1;

    if (m_buffer_id_to_num_slices[buffer_id] == 0 && m_buffer_id_to_num_consumers[buffer_id] == 0) {
        // recycle buffer
        SPDLOG_LOGGER_TRACE(m_logger, "Recycle Buffer {} back to buffer pool from unregister consumer", buffer_id);
        Buffer* buffer = m_buffer_id_to_buffer[buffer_id];
        m_buffer_pool->recycle_buffer(buffer);

        m_buffer_id_to_buffer.erase(buffer_id);
        m_buffer_id_to_num_consumers.erase(buffer_id);
        m_buffer_id_to_num_slices.erase(buffer_id);
    }
}

// -------------------
// Buffer slice
// -------------------
void BufferPoolManager::register_buffer_slice(int buffer_id) {
    std::unique_lock<std::mutex> global_lck(m_buffer_manager_mtx);

    if (m_buffer_id_to_num_slices.find(buffer_id) == m_buffer_id_to_num_slices.end()) {
        throw std::runtime_error("BufferPoolManager Cannot find buffer_id " + std::to_string(buffer_id) + 
                                    ", it may not be registered");
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Register buffer slice to Buffer {}", std::to_string(buffer_id));
    m_buffer_id_to_num_slices[buffer_id] += 1;
}

void BufferPoolManager::unregister_buffer_slice(int buffer_id) {
    std::unique_lock<std::mutex> global_lck(m_buffer_manager_mtx);

    if (m_buffer_id_to_num_slices.find(buffer_id) == m_buffer_id_to_num_slices.end()) {
        throw std::runtime_error("BufferPoolManager Cannot find buffer_id " + std::to_string(buffer_id) + 
                                    ", it may not be registered");
    }
    int cur_num_slices = m_buffer_id_to_num_slices[buffer_id];
    if (cur_num_slices == 0) {
        throw std::runtime_error("Number of register slices is zero for buffer_id " + std::to_string(buffer_id));
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Unregister buffer slice from Buffer {}", std::to_string(buffer_id));
    m_buffer_id_to_num_slices[buffer_id] -= 1;

    if (m_buffer_id_to_num_slices[buffer_id] == 0 && m_buffer_id_to_num_consumers[buffer_id] == 0) {
        // recycle buffer
        SPDLOG_LOGGER_TRACE(m_logger, "Recycle Buffer {} back to buffer pool from unregister slice", buffer_id);
        Buffer* buffer = m_buffer_id_to_buffer[buffer_id];
        m_buffer_pool->recycle_buffer(buffer);

        m_buffer_id_to_buffer.erase(buffer_id);
        m_buffer_id_to_num_consumers.erase(buffer_id);
        m_buffer_id_to_num_slices.erase(buffer_id);
    }
}

// -------------------
// Management operation
// -------------------
void BufferPoolManager::register_buffer(Buffer* buffer) {
    std::unique_lock<std::mutex> global_lck(m_buffer_manager_mtx);

    if (buffer == nullptr) {
        throw std::invalid_argument("Buffer is null");
    }
    // Register an un-used buffer, which can be released at any time
    m_buffer_id_to_buffer.insert(std::make_pair(buffer->get_buffer_id(), buffer));
    m_buffer_id_to_num_consumers[buffer->get_buffer_id()] = 0;
    m_buffer_id_to_num_slices[buffer->get_buffer_id()] = 0;
}


void BufferPoolManager::release_buffer(int buffer_id) {
    // check if the buffer is still in use?
    if (m_buffer_id_to_num_slices.find(buffer_id) == m_buffer_id_to_num_slices.end() || 
        m_buffer_id_to_num_consumers.find(buffer_id) == m_buffer_id_to_num_consumers.end()) {
        throw std::runtime_error("BufferPoolManager Cannot find buffer_id " + std::to_string(buffer_id) + 
                                    ", it may not be registered");        
    }
    if (m_buffer_id_to_num_slices[buffer_id] == 0 && m_buffer_id_to_num_consumers[buffer_id] == 0) {
        // the buffer is unused right now can free it
        Buffer* buffer = m_buffer_id_to_buffer[buffer_id];

        SPDLOG_LOGGER_TRACE(m_logger, "Recycle Buffer {} back to buffer pool", buffer_id);
        m_buffer_pool->recycle_buffer(buffer);

        // unregister buffer from BufferPoolManager
        int buffer_id = buffer->get_buffer_id();
        m_buffer_id_to_buffer.erase(buffer_id);
        m_buffer_id_to_num_consumers.erase(buffer_id);
        m_buffer_id_to_num_slices.erase(buffer_id);
    }
}