/**
 * The ChannelSelectorResultWriter extends the ResultWriter. The emit operation of a record
 * is based on a ChannelSelector to select the target channel.
 */
#pragma once

#include "ResultWriter.hpp"
#include "ChannelSelector.hpp"

template <class T>
class ChannelSelectorResultWriter : public ResultWriter<T>
{
private:
    std::shared_ptr<ChannelSelector<T>>             m_channel_selector;
public:
    ChannelSelectorResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name, std::shared_ptr<ChannelSelector<T>> channel_selector, long timeout)
    : ResultWriter<T>(result_partition, task_name, timeout), m_channel_selector(channel_selector){
        this->m_channel_selector->setup(this->get_number_of_channels());
    }

    /**
      This API has depercated
     */
    ChannelSelectorResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name, std::shared_ptr<ChannelSelector<T>> channel_selector)
    : ResultWriter<T>(result_partition, task_name), m_channel_selector(channel_selector){
        throw std::runtime_error("ChannelSelectorResultWriter need flush timeout, this API is depercated!");
    }

    /* Implement emit record with channel selector */
    void                                            emit(StreamRecordV2<T>* record) {
        ResultWriter<T>::emit(record, this->m_channel_selector->select_channel(record));
    }
};