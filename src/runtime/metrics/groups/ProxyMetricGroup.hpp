/**
 * Metric group which forwards all registration calls to its parent metric group.
 */
#pragma once
#include "MetricGroup.hpp"

template <class P> 
class ProxyMetricGroup : public MetricGroup {
protected:
    std::shared_ptr<P>  m_parent_metric_group;
public:
    ProxyMetricGroup(std::shared_ptr<P> parent_matric_group): m_parent_metric_group(parent_matric_group){}

    std::shared_ptr<Counter> counter(std::string name) {
        m_parent_metric_group->counter(name);
    }

    /* Register a Counter */
    std::shared_ptr<Counter> counter(std::string name, std::shared_ptr<Counter> counter) {
        m_parent_metric_group->counter(name, counter);
    }

    /* Register a Meter */
    std::shared_ptr<Meter> meter(std::string name, std::shared_ptr<Meter> meter) {
        m_parent_metric_group->meter(name, meter);
    }

    // ---------------------------------
    // Groups
    // ---------------------------------
    
    /* Creates a new MetricGroup and adds it to this group sub-groups */
    std::shared_ptr<MetricGroup> add_group(std::string name) {
        m_parent_metric_group->add_group(name);
    }

    /* Gets the scope as an array of the scope components */
    std::vector<std::string>& get_scope_components() {
        m_parent_metric_group->get_scope_components();
    }

    /* Return the fully qualified metric name */
    std::string get_metric_identifier(std::string metric_name) {
        m_parent_metric_group->get_metric_identifier(metric_name);
    }
};