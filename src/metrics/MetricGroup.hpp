/**
 * A MetricGroup is a named container for Metrics and further metric subgroups.
 */
#pragma once
#include "Counter.hpp"
#include "Meter.hpp"
#include <memory>
#include <string>
#include <vector>

class MetricGroup
{
public:
    /* Creates and registers a new Counter */
    virtual std::shared_ptr<Counter> counter(std::string name) = 0;

    /* Register a Counter */
    virtual std::shared_ptr<Counter> counter(std::string name, std::shared_ptr<Counter> counter) = 0;

    /* Register a Meter */
    virtual std::shared_ptr<Meter> meter(std::string name, std::shared_ptr<Meter> meter) = 0; 

    // ---------------------------------
    // Groups
    // ---------------------------------
    
    /* Creates a new MetricGroup and adds it to this group sub-groups */
    virtual std::shared_ptr<MetricGroup> add_group(std::string name) = 0;

    /* Gets the scope as an array of the scope components */
    virtual std::vector<std::string>& get_scope_components() = 0;

    /* Return the fully qualified metric name */
    virtual std::string get_metric_identifier(const std::string& metric_name) = 0;

    virtual void close() {};
};

