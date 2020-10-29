/**
 * 
 */
#pragma once

#include "SubpartitionAvailableListener.hpp"
#include "InputChannel.hpp"
#include "BufferPool.hpp"
#include "BufferOrEvent.hpp"
#include "LoggerFactory.hpp"
#include "Counter.hpp"
#include "AvailabilityProvider.hpp"
#include <map>
#include <string>
#include <deque>
#include <mutex>
#include <utility>

class InputChannel;

class InputGate: public AvailabilityProvider, public std::enable_shared_from_this<InputGate>
{
private:
    typedef std::shared_ptr<AvailabilityProvider::AvailabilityHelper> AvailabilityHelperPtr;
    typedef std::shared_ptr<Counter>                    CounterPtr;

    AvailabilityHelperPtr                               m_availability_helper = nullptr;

    CounterPtr                                          m_bytes_in;

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
    static std::shared_ptr<spdlog::logger>              m_logger;

    /**
      Update pass-by bytes
     */
    std::shared_ptr<BufferOrEvent>                      update_metrics(std::shared_ptr<BufferOrEvent> buffer_or_event) {
        if (m_bytes_in != nullptr) {
            m_bytes_in->inc(buffer_or_event->get_buffer()->get_max_capacity());
        }
        return buffer_or_event;
    }

    void                                                mark_available() {
        std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
        CompletableFuturePtr to_notify = m_availability_helper->get_unavailable_to_reset_available();
        channels_with_data_lock.unlock();
        to_notify->complete(true);
    }

public:
    InputGate(int gate_idx):m_gate_idx(gate_idx){}

    InputGate(std::string owning_task_name, int gate_idx, int consumed_subpartition_idx, int number_of_input_channels):
    m_owning_task_name(owning_task_name), m_gate_idx(gate_idx), m_consumed_subpartition_idx(consumed_subpartition_idx), 
    m_number_of_input_channels(number_of_input_channels) {}    

    void setup() {
        if (m_availability_helper == nullptr) {
            m_availability_helper = std::make_shared<AvailabilityProvider::AvailabilityHelper>(shared_from_this());
        }
    }

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
    void                                                set_bytes_in_counter(CounterPtr bytes_in) {m_bytes_in = bytes_in;}

    void                                                set_input_channels(std::shared_ptr<InputChannel>* input_channels, int num_input_channels);           

    /**
      @return a future that is a completed if there are more records available. If there are more 
      records available immediately, AVAILABLE should be returned. Previously returned not completed
      futures should become completed once there are more records available.
     */
    CompletableFuturePtr                                get_available_future() override {
        return m_availability_helper->get_available_future();
    }
};
