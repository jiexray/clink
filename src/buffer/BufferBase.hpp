#pragma once
#include <memory>

class BufferBase
{
private:
public:
    /* Slice the buffer */
    virtual int                             get_buffer_offset() = 0;
    // virtual BufferBase* read_only_slice(int, int) = 0;
    virtual std::shared_ptr<BufferBase>     read_only_slice(int, int) = 0;

    /* Properties */
    virtual int                             get_max_capacity() = 0;
    virtual int                             get_buffer_id() = 0;

    /* Random access to buffer */
    virtual int                             get(unsigned char*, int) = 0;
    virtual int                             get(unsigned char*, int, int) = 0;
    virtual void                            release() = 0;
};