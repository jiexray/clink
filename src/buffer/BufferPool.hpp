/**
 * A pool for buffer.
 */
#pragma once

#include <vector>
#include "Buffer.hpp"
#include "BufferBuilder.hpp"
#include <mutex>
#include <memory>
#include <condition_variable>
#include <stdexcept>

class BufferPool
{
private:
    int                             m_number_of_buffers;
    int                             m_current_pool_size; // no use, in future, may be used for dynamical pool size

    std::vector<Buffer*>            available_buffers;
    std::mutex                      mtx_available_buffer;
    std::mutex                      available_helper;
    std::condition_variable         available_condition_variable;

    Buffer*                         request_buffer();
    Buffer*                         request_buffer_blocking();
public:
    BufferPool(int number_of_buffers_to_allocate, int buffer_size);
    ~BufferPool() {};

    /* Properties */
    int                             get_number_of_available_buffers() {return available_buffers.size();}
    int                             get_max_number_of_buffers() {return m_number_of_buffers;}

    /* request / recycle BufferBuilder */
    std::shared_ptr<BufferBuilder>  request_buffer_builder();
    std::shared_ptr<BufferBuilder>  request_buffer_builder_blocking();
    void                            recycle(std::shared_ptr<BufferBuilder> buffer_builder);
};

/**
 * Initialize BufferPool with number_of_buffers_to_allocate Buffers,
 * Each buffer is buffer_size in size.
 */
inline BufferPool::BufferPool(int number_of_buffers_to_allocate, int buffer_size):
m_number_of_buffers(number_of_buffers_to_allocate),
m_current_pool_size(number_of_buffers_to_allocate) {
    available_buffers.resize(number_of_buffers_to_allocate);

    for (int i = 0; i < number_of_buffers_to_allocate; i++) {
        available_buffers[i] = new Buffer(buffer_size);
    }
}

/**
 * Return a buffer instance from the buffer pool, if one is available.
 */
inline Buffer* BufferPool::request_buffer() {
    Buffer* buffer = nullptr;
    
    mtx_available_buffer.lock();
    if (available_buffers.empty()) {
        mtx_available_buffer.unlock();
        return buffer;
    }

    // poll one buffer from available_buffers
    buffer = available_buffers.back();
    available_buffers.pop_back();
    mtx_available_buffer.unlock();

    return buffer;
}

inline Buffer* BufferPool::request_buffer_blocking() {
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

inline std::shared_ptr<BufferBuilder> BufferPool::request_buffer_builder() {
    Buffer* buffer = request_buffer();

    if (buffer == nullptr) {
        return nullptr;
    }

    return std::make_shared<BufferBuilder>(buffer);
}

inline std::shared_ptr<BufferBuilder> BufferPool::request_buffer_builder_blocking() {
    Buffer* buffer = request_buffer_blocking();

    if (buffer == nullptr) {
        throw std::runtime_error( "Blocking mode request cannot return a null buffer" );
    }

    return std::make_shared<BufferBuilder>(buffer);
}

inline void BufferPool::recycle(std::shared_ptr<BufferBuilder> buffer_builder) {
    mtx_available_buffer.lock();
    available_buffers.push_back(buffer_builder->get_buffer());
    mtx_available_buffer.unlock();

    available_condition_variable.notify_all();
}

