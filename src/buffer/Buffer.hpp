/**
 * Buffer holds the data block of multiple tuples, which need to be transmitted between consequtive tasks.
 */
#pragma once

#include <string>
#include <iostream>
#include "ReadOnlySlidedBuffer.hpp"
#include "BufferPoolManager.hpp"
#include "BufferBase.hpp"
#include <stdexcept>
#include <memory>

class BufferPoolManager;

class Buffer : public BufferBase
{
private:
    char*                                   m_data;
    int                                     m_size;
    int                                     m_buffer_id;
    std::shared_ptr<BufferPoolManager>      m_buffer_pool_manager;

    static int                              m_id_counter;
    static std::shared_ptr<spdlog::logger>  m_logger;


public:
    Buffer();
    Buffer(int capacity);
    Buffer(int capacity, std::shared_ptr<BufferPoolManager> buffer_pool_mananger);
    ~Buffer() {}

    void                                    free();

    /* Properties */

    /**
      Get the total capacity of a buffer. If it is a ReadOnlySlideBuffer, this is its size.
     */
    int                                     get_max_capacity() override {return m_size;}
    int                                     get_buffer_id() override {return m_buffer_id;}
    std::shared_ptr<BufferPoolManager>      get_buffer_pool_manager() {return m_buffer_pool_manager;}

    /* Random Access get() and put() methods */
    int                                     get(char* buf, int index) override;
    void                                    put(int index, char value);

    /* Slice the buffer, override from BufferBase */
    int                                     get_buffer_offset() override {return 0;}
    // BufferBase*                             read_only_slice(int offset, int length) override;
    std::shared_ptr<BufferBase>             read_only_slice(int offset, int length) override;

    void                                    release() {throw std::runtime_error("Buffer do not support release()");}

    static int                              get_next_id() {return m_id_counter++;}
};