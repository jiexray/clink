/**
 * Base interface for custom metric reporters
 */
#pragma once

#include "MetricReporter.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <iostream>

class AbstractMetricReporter: public MetricReporter
{
protected:
    typedef std::shared_ptr<Counter>    CounterPtr;
    typedef std::shared_ptr<Meter>      MeterPtr;

    std::map<std::string, CounterPtr>   m_counters;
    std::map<std::string, MeterPtr>     m_meters;
    
    std::mutex                          m_global_mtx;
public:
    void                                notify_of_added_metric(std::shared_ptr<Metric> metric, 
                                                               std::string metric_name, 
                                                               std::shared_ptr<MetricGroup> group) override {
        std::string name = group->get_metric_identifier(metric_name);
        std::unique_lock<std::mutex> global_lock(m_global_mtx);
        if (std::dynamic_pointer_cast<Counter>(metric) != nullptr) {
            // a counter
            m_counters.insert(std::make_pair(name, std::dynamic_pointer_cast<Counter>(metric)));
        } else if (std::dynamic_pointer_cast<Meter>(metric) != nullptr) {
            // a meter
            m_meters.insert(std::make_pair(name, std::dynamic_pointer_cast<Meter>(metric)));
        } else {
            throw std::invalid_argument("Unknown type of Metric, except for Counter and Meter");
        }
    }

    void                                notify_of_removed_metric(std::shared_ptr<Metric> metric, 
                                                                 std::string metric_name, 
                                                                 std::shared_ptr<MetricGroup> group) {
        std::string name = group->get_metric_identifier(metric_name);
        std::unique_lock<std::mutex> global_lock(m_global_mtx);
        if (std::dynamic_pointer_cast<Counter>(metric) != nullptr) {
            // a counter
            m_counters.erase(name);
        } else if (std::dynamic_pointer_cast<Meter>(metric) != nullptr) {
            // a meter
            m_meters.erase(name);
        } else {
            throw std::invalid_argument("Unknown type of Metric, except for Counter and Meter");
        }
    }
};

