/**
 * Reporters are used to export Metrics to an external backend.
 */
#pragma once
#include "Metric.hpp"
#include "MetricGroup.hpp"
#include "MetricConfig.hpp"
#include <string>

class MetricReporter
{
public:
    virtual void open(std::shared_ptr<MetricConfig> config) = 0;
    virtual void close() = 0;

    virtual void notify_of_added_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) = 0;
    virtual void notify_of_removed_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) = 0;
};