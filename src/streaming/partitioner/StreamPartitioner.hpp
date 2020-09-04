/**
 * ChannelSelector for use in streaming programs.
 */
#pragma once
#include "../../result/ChannelSelector.hpp"
#include "../../streamrecord/StreamRecord.hpp"
#include <string>

template <class T>
class StreamPartitioner : public ChannelSelector<T>
{
protected:
    int                     m_number_of_channels;
public:
    void                    setup(int number_of_channels) {m_number_of_channels = number_of_channels;}
    virtual std::string     to_string() = 0;      

    /* Properties */
    int                     get_number_of_channels() {return m_number_of_channels;}  
};

