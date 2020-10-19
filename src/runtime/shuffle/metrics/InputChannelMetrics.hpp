/**
 * Collects metrics for RemoteInputChannel and LocalInputChannel
 */
#pragma once
#include "MetricGroup.hpp"
#include "MetricNames.hpp"
#include "MeterView.hpp"
#include <vector>

class InputChannelMetrics
{
private:
    typedef std::shared_ptr<Counter>        CounterPtr;
    typedef std::shared_ptr<MetricGroup>    MetricGroupPtr;

    const static std::string    IO_NUM_BYTES_IN_LOCAL;
    const static std::string    IO_NUM_BYTES_IN_REMOTE;
    const static std::string    IO_NUM_BUFFERS_IN_LOCAL;
    const static std::string    IO_NUM_BUFFERS_IN_REMOTE;

    CounterPtr                  m_bytes_in_local;
    CounterPtr                  m_bytes_in_remote;
    CounterPtr                  m_buffers_in_local;
    CounterPtr                  m_buffers_in_remote;
public:
    InputChannelMetrics(MetricGroupPtr parent) {
        m_bytes_in_local     = create_counter(IO_NUM_BYTES_IN_LOCAL, parent);
        // TODO: support remote data transfer
        // m_bytes_in_remote    = create_counter(IO_NUM_BYTES_IN_REMOTE, parent);
        m_buffers_in_local  = create_counter(IO_NUM_BUFFERS_IN_LOCAL, parent);
        // m_buffers_in_remote = create_counter(IO_NUM_BUFFERS_IN_REMOTE, parent);
    }

    static CounterPtr           create_counter(const std::string& name, MetricGroupPtr parent) {
        CounterPtr counter = parent->counter(name);
        parent->meter(name + MetricNames::SUFFIX_RATE, std::make_shared<MeterView>(counter));

        return counter;
    }

    /* Properties */
    CounterPtr                  get_bytes_in_local() {return m_bytes_in_local;}
    CounterPtr                  get_bytes_in_remote() {return m_bytes_in_remote;}
    CounterPtr                  get_buffers_in_local() {return m_buffers_in_local;}
    CounterPtr                  get_buffers_in_remote() {return m_buffers_in_remote;}
};
