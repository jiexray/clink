/**
 * The ChannelSelector determines to which @logical@ channels a record should be written to.
 */
#pragma once
#include "../streamrecord/StreamRecord.hpp"
#include <memory>

template<class T>
class ChannelSelector
{
public:
    virtual int     select_channel(std::shared_ptr<StreamRecord<T>> record) = 0;
    virtual void    setup(int number_of_channel) = 0;
};
