/**
 * Metric group that contains sharalbe pre-defined IO-related metrics. The metrics registration is forwarded to the parent task metric group.
 */
#pragma once
#include "TaskMetricGroup.hpp"
#include "ProxyMetricGroup.hpp"
#include "SimpleCounter.hpp"
#include "MetricNames.hpp"

class TaskMetricGroup;

class TaskIOMetricGroup: public ProxyMetricGroup<TaskMetricGroup>
{
private:
    class SumCounter: public SimpleCounter
    {
    private:
        std::vector<std::shared_ptr<Counter>> m_internal_counters;
    public:
        void add_counter(std::shared_ptr<Counter> to_add) {
            m_internal_counters.push_back(to_add);
        }

        long get_counter() {
            long sum = get_count();
            for (std::shared_ptr<Counter> counter: m_internal_counters) {
                sum += counter->get_count();
            }
            return sum;
        } 
    };
    typedef std::shared_ptr<Counter> CounterPtr;
    typedef std::shared_ptr<Meter> MeterPtr;
    typedef std::shared_ptr<SumCounter> SumCounterPtr;
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;

    CounterPtr                      m_bytes_in;
    CounterPtr                      m_bytes_out;
    SumCounterPtr                   m_records_in;
    SumCounterPtr                   m_records_out;
    CounterPtr                      m_buffers_out;

    MeterPtr                        m_bytes_in_rate;
    MeterPtr                        m_bytes_out_rate;
    MeterPtr                        m_records_in_rate;
    MeterPtr                        m_records_out_rate;
    MeterPtr                        m_buffers_out_rate;
    MeterPtr                        m_idle_time_per_second;                                
public:
    TaskIOMetricGroup(TaskMetricGroupPtr parent);

    /* Properties */
    CounterPtr get_bytes_in_counter() {return m_bytes_in;}
    CounterPtr get_bytes_out_counter() {return m_bytes_out;}
    CounterPtr get_records_in_counter() {return m_records_in;}
    CounterPtr get_records_out_counter() {return m_records_out;}
    CounterPtr get_buffers_out_counter() {return m_buffers_out;}

    MeterPtr   get_idle_time_ms_per_second() {return m_idle_time_per_second;}
};
