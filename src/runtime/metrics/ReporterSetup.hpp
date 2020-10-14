/**
 * Encapsulates everything needed for the instantiation and configuration of a MetricReporter
 */
#pragma once

#include "MetricReporter.hpp"
#include "MetricConfig.hpp"
#include <memory>
#include <string>

class ReporterSetup
{
private:
    std::string                     m_name;
    std::shared_ptr<MetricConfig>   m_configuration;
    std::shared_ptr<MetricReporter> m_reporter;
public:
    ReporterSetup(std::string name, std::shared_ptr<MetricConfig> config, std::shared_ptr<MetricReporter> reporter):
                m_name(name), m_configuration(config), m_reporter(reporter){}

    /* Properties */
    std::string                     get_name() {return m_name;}
    std::shared_ptr<MetricConfig>   get_configuration() {return m_configuration;}
    std::shared_ptr<MetricReporter> get_reporter() {return m_reporter;}

    
};
