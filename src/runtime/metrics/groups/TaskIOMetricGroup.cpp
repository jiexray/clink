#include "TaskIOMetricGroup.hpp"

TaskIOMetricGroup::TaskIOMetricGroup(TaskMetricGroupPtr parent): ProxyMetricGroup<TaskMetricGroup>(parent) {
    m_bytes_in = parent->counter(MetricNames::IO_NUM_BYTES_IN);
    m_bytes_out = parent->counter(MetricNames::IO_NUM_BYTES_OUT);
    m_bytes_in_rate = parent->meter(MetricNames::IO_NUM_BYTES_IN_RATE, std::make_shared<MeterView>(m_bytes_in));
    m_bytes_out_rate = parent->meter(MetricNames::IO_NUM_BYTES_OUT_RATE, std::make_shared<MeterView>(m_bytes_out));

    // Note: no use in Task
    // m_records_in = std::dynamic_pointer_cast<SumCounter>(parent->counter(MetricNames::IO_NUM_RECORDS_IN, std::make_shared<SumCounter>()));
    // m_records_out = std::dynamic_pointer_cast<SumCounter>(parent->counter(MetricNames::IO_NUM_RECORDS_OUT, std::make_shared<SumCounter>()));
    // m_records_in_rate = parent->meter(MetricNames::IO_NUM_RECORDS_IN_RATE, std::make_shared<MeterView>(m_records_in));
    // m_records_out_rate = parent->meter(MetricNames::IO_NUM_RECORDS_OUT_RATE, std::make_shared<MeterView>(m_records_out));

    m_buffers_out = parent->counter(MetricNames::IO_NUM_BUFFERS_OUT);
    m_buffers_out_rate = parent->meter(MetricNames::IO_NUM_BUFFERS_OUT_RATE, std::make_shared<MeterView>(m_buffers_out));

    m_idle_time_per_second = parent->meter(MetricNames::TASK_IDLE_TIME, std::make_shared<MeterView>(std::make_shared<SimpleCounter>()));
}