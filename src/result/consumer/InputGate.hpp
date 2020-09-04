/**
 * 
 */
#pragma once

#include "../SubpartitionAvailableListener.hpp"
#include "InputChannel.hpp"
#include "../../buffer/BufferPool.hpp"
#include "BufferOrEvent.hpp"
#include <map>
#include <string>
#include <deque>
#include <mutex>
#include <utility>

class InputChannel;

class InputGate 
{
private:
    std::string                                         m_owning_task_name;
    int                                                 m_gate_idx;

    int                                                 m_number_of_input_channels;
    int                                                 m_consumed_subpartition_idx;
    /* A map from partition_idx to InputChannel */
    std::map<int, std::shared_ptr<InputChannel>>        m_input_channels;

    /* Channels, which notifies this input gate about available data */
    std::deque<std::shared_ptr<InputChannel>>           m_input_channels_with_data;
    /* Mutex and condition variable for accessing m_input_channels_with_data */
    std::mutex                                          m_input_channels_with_data_mtx;
    std::mutex                                          available_helper;
    std::condition_variable                             available_condition_variable;
    std::mutex                                          m_request_mtx; // lock to guard partition requests and runtime channel updates
    /* Channel notification */
    void                                                queue_channel(std::shared_ptr<InputChannel> channel);
    

    // TODO: add a bitset for input_channels_with_data

    // TODO: buffer pool for network data transfer, currently only local, no use
    std::shared_ptr<BufferPool>                         m_buffer_pool;

public:
    InputGate(int gate_idx):m_gate_idx(gate_idx){}
    InputGate(std::string owning_task_name, int gate_idx, int consumed_subpartition_idx, int number_of_input_channels):
    m_owning_task_name(owning_task_name), m_gate_idx(gate_idx), m_consumed_subpartition_idx(consumed_subpartition_idx), 
    m_number_of_input_channels(number_of_input_channels) {}    

    /* Request subpartitions, connect input channels to the corresponding subpartitions via subpartition view */
    void                                                request_partitions();  

    /* Consume data */
    std::shared_ptr<BufferOrEvent>                      poll_next(); 

    /* Notify data available */
    void                                                notify_channel_non_empty(std::shared_ptr<InputChannel> channel);
    std::shared_ptr<InputChannel>                       get_channel(bool blocking);

    /* Properties */
    int                                                 get_gate_idx() {return m_gate_idx;}
    std::map<int, std::shared_ptr<InputChannel>>&       get_input_channels() {return m_input_channels;}
    int                                                 get_number_of_input_channels() {return m_number_of_input_channels;}

    void                                                set_input_channels(std::shared_ptr<InputChannel>* input_channels, int num_input_channels);           
};
