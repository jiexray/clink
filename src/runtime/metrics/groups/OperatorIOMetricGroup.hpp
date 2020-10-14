/**
 * Metric group that contains sharable pre-define IO-related metrics.
 */
#pragma once
#include "ComponentMetricGroup.hpp"
#include "OperatorMetricGroup.hpp"
#include "Counter.hpp"
#include "Meter.hpp"
#include "MeterView.hpp"
#include "MetricNames.hpp"

class OperatorMetricGroup;

class OperatorIOMetricGroup: ProxyMetricGroup<OperatorMetricGroup>
{
private:
    std::shared_ptr<Counter>    m_records_in;
    std::shared_ptr<Counter>    m_records_out;
    std::shared_ptr<Meter>      m_records_in_rate;
    std::shared_ptr<Meter>      m_records_out_rate;
public:
    OperatorIOMetricGroup(std::shared_ptr<OperatorMetricGroup> parent_metric_group);

    /* Properties */
    std::shared_ptr<Counter> get_records_in_counter() {return m_records_in;}
    std::shared_ptr<Counter> get_records_out_counter() {return m_records_out;}
};
