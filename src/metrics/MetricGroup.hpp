/**
 * A MetricGroup is a named container for Metrics and further metric subgroups.
 */
#pragma once
#include "Counter.hpp"
#include "Meter.hpp"
#include "Gauge.hpp"
#include "TemplateHelper.hpp"
#include <memory>
#include <string>
#include <vector>

class MetricGroup
{
public:
    /* Creates and registers a new Counter */
    virtual std::shared_ptr<Counter> counter(const std::string& name) = 0;

    /* Register a Counter */
    virtual std::shared_ptr<Counter> counter(const std::string& name, std::shared_ptr<Counter> counter) = 0;

    /* Register a Meter */
    virtual std::shared_ptr<Meter> meter(const std::string& name, std::shared_ptr<Meter> meter) = 0; 

    /* Register a Gauge */
    virtual std::shared_ptr<Gauge> gauge(const std::string& name, std::shared_ptr<Gauge> guage) = 0;

    // ---------------------------------
    // Groups
    // ---------------------------------
    
    /* Creates a new MetricGroup and adds it to this group sub-groups */
    virtual std::shared_ptr<MetricGroup> add_group(const std::string& name) = 0;

    /* Gets the scope as an array of the scope components */
    virtual std::vector<std::string>& get_scope_components() = 0;

    /* Return the fully qualified metric name */
    virtual std::string get_metric_identifier(const std::string& metric_name) = 0;

    virtual void close() {};
};

