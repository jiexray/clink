#include "BufferPool.hpp"
int BufferPool::m_buffer_pool_id_counter = 0;

std::shared_ptr<spdlog::logger> BufferPool::m_logger = LoggerFactory::get_logger("BufferPool");

/**
 * Initialize BufferPool with number_of_buffers_to_allocate Buffers,
 * Each buffer is buffer_size in size.
 */
BufferPool::BufferPool(int number_of_buffers_to_allocate, int buffer_size):
m_number_of_buffers(number_of_buffers_to_allocate),
m_current_pool_size(number_of_buffers_to_allocate) {
    m_buffer_pool_id = get_next_id();
    m_buffer_pool_manager = std::make_shared<BufferPoolManager>(this);

    available_buffers.resize(number_of_buffers_to_allocate);

    for (int i = 0; i < number_of_buffers_to_allocate; i++) {
        available_buffers[i] = new Buffer(buffer_size, m_buffer_pool_manager);
    }

    // setup logger
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);

    // setup BufferPoolMananger
}

/**
 * Return a buffer instance from the buffer pool, if one is available.
 */
Buffer* BufferPool::request_buffer() {
    Buffer* buffer = nullptr;
    
    mtx_available_buffer.lock();
    if (available_buffers.empty()) {
        mtx_available_buffer.unlock();
        return buffer;
    }

    // poll one buffer from available_buffers
    buffer = available_buffers.back();
    available_buffers.pop_back();

    // NOTE: Must free lock before register_buffer, will have a dead-lock.
    //       For register_buffer need lock
    mtx_available_buffer.unlock();

    // register the requested buffer to BufferPoolManager
    if (m_buffer_pool_manager != nullptr){
        m_buffer_pool_manager->register_buffer(buffer);
    }
    SPDLOG_LOGGER_DEBUG(m_logger, "BufferPool::request_buffer(), request Buffer {} available buffers in BufferPool {}: {}",
                                 buffer->get_buffer_id(), m_buffer_pool_id, available_buffers.size());

    return buffer;
}

Buffer* BufferPool::request_buffer_blocking() {
    Buffer* buffer = nullptr;
    buffer = request_buffer();

    if (buffer != nullptr) {
        return buffer;
    }

    // start to wait for buffer available
    while (true) {
        std::unique_lock<std::mutex> available_helper_lock(available_helper);
        available_condition_variable.wait(available_helper_lock);
        buffer = request_buffer();
        if (buffer != nullptr) {
            return buffer;
        }
    }
}

std::shared_ptr<BufferBuilder> BufferPool::request_buffer_builder() {
    Buffer* buffer = request_buffer();

    if (buffer == nullptr) {
        return nullptr;
    }

    return std::make_shared<BufferBuilder>(buffer);
}

std::shared_ptr<BufferBuilder> BufferPool::request_buffer_builder_blocking() {
    Buffer* buffer = request_buffer_blocking();

    if (buffer == nullptr) {
        throw std::runtime_error( "Blocking mode request cannot return a null buffer" );
    }

    return std::make_shared<BufferBuilder>(buffer);
}

/**
 * Deprecated
 */
void BufferPool::recycle(std::shared_ptr<BufferBuilder> buffer_builder) {
    mtx_available_buffer.lock();
    available_buffers.push_back(buffer_builder->get_buffer());
    SPDLOG_LOGGER_DEBUG(m_logger, "BufferPool::recycle() available buffer: {}", available_buffers.size());
    mtx_available_buffer.unlock();

    available_condition_variable.notify_all();
}


void BufferPool::recycle_buffer(Buffer* buffer) {
    mtx_available_buffer.lock();
    available_buffers.push_back(buffer);
    SPDLOG_LOGGER_DEBUG(m_logger, "BufferPool::recycle_buffer(), recycle Buffer {} available buffer in BufferPool {}: {}", 
                            buffer->get_buffer_id(), m_buffer_pool_id, available_buffers.size());
    mtx_available_buffer.unlock();

    available_condition_variable.notify_all();
}