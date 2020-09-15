/**
 * Either type for Buffer or AbstractEvent instance tagged with the channel index, from which they were received.
 */
#pragma once
#include "../../buffer/BufferBase.hpp"
#include <memory>

class BufferOrEvent
{
private:
    std::shared_ptr<BufferBase>         m_buffer;

    bool                                m_more_available;
    int                                 m_channel_idx;
public:
    BufferOrEvent(std::shared_ptr<BufferBase> buffer, int channel_idx, bool more_available):
    m_buffer(buffer), m_more_available(more_available), m_channel_idx(channel_idx) {}
    
    /* Properties */
    std::shared_ptr<BufferBase>         get_buffer() {return m_buffer;}
    int                                 get_channel_idx() {return m_channel_idx;}
    bool                                more_available() {return m_more_available;}
    bool                                is_buffer() {return m_buffer != nullptr;}


};

