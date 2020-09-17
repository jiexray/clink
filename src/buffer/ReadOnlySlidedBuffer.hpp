/**
 * Read-only sliced Buffer implementation wrapping a Buffer's sub-region.
 */
#pragma once

#include "Buffer.hpp"
#include "BufferBase.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <memory>

class Buffer;

class ReadOnlySlidedBuffer : public BufferBase
{
private:
    int         m_buffer_offset;
    int         m_size;

    /* parent buffer */
    Buffer*     parent_buffer;
    
    static std::shared_ptr<spdlog::logger> m_logger;
public:
    ReadOnlySlidedBuffer(BufferBase* buffer, int offset, int length);
    ~ReadOnlySlidedBuffer();

    /* Slice the buffer, override from BufferBase */
    int                             get_buffer_offset() override;
    // BufferBase* read_only_slice(int offset, int length) override;
    std::shared_ptr<BufferBase>     read_only_slice(int offset, int length);

    /* Properties */
    int                             get_max_capacity() override {return m_size;}
    int                             get_buffer_id() override;

    /* Random access to data */
    int                             get(char* buf, int index) override;
    void                            release();
};