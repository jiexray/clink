#include "OperatorIOMetricGroup.hpp"

OperatorIOMetricGroup::OperatorIOMetricGroup(std::shared_ptr<OperatorMetricGroup> parent_metric_group): ProxyMetricGroup<OperatorMetricGroup>(parent_metric_group) {
    m_records_in = parent_metric_group->counter(MetricNames::IO_NUM_RECORDS_IN);
    m_records_out = parent_metric_group->counter(MetricNames::IO_NUM_RECORDS_OUT);
    m_records_in_rate = parent_metric_group->meter(MetricNames::IO_NUM_RECORDS_IN_RATE, std::make_shared<MeterView>(m_records_in));
    m_records_out_rate = parent_metric_group->meter(MetricNames::IO_NUM_RECORDS_OUT_RATE, std::make_shared<MeterView>(m_records_out));
}