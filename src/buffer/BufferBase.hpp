#pragma once

class BufferBase
{
private:
public:
    /* Slice the buffer */
    virtual int         get_buffer_offset() = 0;
    virtual BufferBase* read_only_slice(int, int) = 0;

    /* Properties */
    virtual int         get_max_capacity() = 0;

    /* Random access to buffer */
    virtual int         get(char*, int) = 0;
};