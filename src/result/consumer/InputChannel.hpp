/**
 * An input channel consume a single ResultSubpartitionView.
 */
#pragma once
#include "InputChannelInfo.hpp"
#include "InputGate.hpp"
#include <memory>
#include "ResultPartitionManager.hpp"
#include "ResultSubpartitionView.hpp"
#include "Counter.hpp"
#include "LoggerFactory.hpp"

class InputGate;

class InputChannel: public SubpartitionAvailableListener, public std::enable_shared_from_this<InputChannel>
{
private:
    std::shared_ptr<InputChannelInfo>           m_channel_info;
    std::string                                 m_partition_id;
    std::shared_ptr<InputGate>                  m_input_gate;
    std::shared_ptr<ResultPartitionManager>     m_partition_manager;
    std::shared_ptr<ResultSubpartitionView>     m_subpartition_view;

    static std::shared_ptr<spdlog::logger>      m_logger;

protected:
    typedef std::shared_ptr<Counter>            CounterPtr;
    CounterPtr                                  m_bytes_in;
    CounterPtr                                  m_buffers_in;


public:
    InputChannel(std::shared_ptr<InputGate> input_gate, int channel_idx, std::string partition_id,
                std::shared_ptr<ResultPartitionManager> partition_manager): 
                InputChannel(input_gate, channel_idx, partition_id, partition_manager, nullptr, nullptr) {}

    InputChannel(std::shared_ptr<InputGate> input_gate, int channel_idx, std::string partition_id,
                std::shared_ptr<ResultPartitionManager> partition_manager,
                CounterPtr bytes_in, CounterPtr buffers_in);

    void                                        notify_data_available();
    void                                        request_subpartition(int subpartition_idx);
    std::shared_ptr<BufferAndBacklog>           get_next_buffer();

    /* Properties */
    std::shared_ptr<ResultSubpartitionView>     get_subpartition_view() {return m_subpartition_view;}
    int                                         get_channel_idx() {return m_channel_info->get_input_channel_idx();}
    std::string                                 get_partition_id() {return m_partition_id;}
};

