/**
 * A pool for buffer.
 */
#pragma once

#include <vector>
#include "Buffer.hpp"
#include "BufferBuilder.hpp"
#include "Constant.hpp"
#include "BufferPoolManager.hpp"
#include <mutex>
#include <memory>
#include <condition_variable>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

class BufferPoolManager;
class Buffer;
class BufferBuilder;

class BufferPool: public std::enable_shared_from_this<BufferPool>
{
private:
    int                                     m_number_of_buffers;
    int                                     m_current_pool_size; // no use, in future, may be used for dynamical pool size

    std::vector<Buffer*>                    available_buffers;
    std::mutex                              mtx_available_buffer;
    std::mutex                              available_helper;
    std::condition_variable                 available_condition_variable;

    Buffer*                                 request_buffer();
    Buffer*                                 request_buffer_blocking();
    std::shared_ptr<BufferPoolManager>      m_buffer_pool_manager;

    static std::shared_ptr<spdlog::logger>  m_logger;
public:
    BufferPool(int number_of_buffers_to_allocate, int buffer_size);
    ~BufferPool() {};

    /* Properties */
    int                                     get_number_of_available_buffers() {return available_buffers.size();}
    int                                     get_max_number_of_buffers() {return m_number_of_buffers;}
    std::shared_ptr<BufferPoolManager>      get_buffer_pool_mananger() {return m_buffer_pool_manager;}

    /* request / recycle BufferBuilder */
    std::shared_ptr<BufferBuilder>          request_buffer_builder();
    std::shared_ptr<BufferBuilder>          request_buffer_builder_blocking();
    void                                    recycle(std::shared_ptr<BufferBuilder> buffer_builder);
    void                                    recycle_buffer(Buffer* buffer);
};
