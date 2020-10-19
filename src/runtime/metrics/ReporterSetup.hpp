/**
 * Encapsulates everything needed for the instantiation and configuration of a MetricReporter
 */
#pragma once

#include "MetricReporter.hpp"
#include "MetricConfig.hpp"
#include "Configuration.hpp"
#include "LoggerReporter.hpp"
#include <memory>
#include <string>
#include <vector>

class Configuration;

class ReporterSetup
{
private:
    typedef std::shared_ptr<ReporterSetup>  ReporterSetupPtr;
    typedef std::shared_ptr<Configuration>  ConfigurationPtr;
    typedef std::shared_ptr<MetricConfig>   MetricConfigPtr;
    typedef std::shared_ptr<MetricReporter> MetricReporterPtr;

    std::string                                     m_name;
    MetricConfigPtr                                 m_configuration;
    MetricReporterPtr                               m_reporter;

    static ReporterSetupPtr                         create_reporter_setup(const std::string& reporter_name, MetricConfigPtr metric_config, MetricReporterPtr reporter) {
        reporter->open(metric_config);
        return std::make_shared<ReporterSetup>(reporter_name, metric_config, reporter);
    }
public:
    ReporterSetup(std::string name, std::shared_ptr<MetricConfig> config, std::shared_ptr<MetricReporter> reporter):
                m_name(name), m_configuration(config), m_reporter(reporter){}

    /* Properties */
    std::string                                     get_name() {return m_name;}
    std::shared_ptr<MetricConfig>                   get_configuration() {return m_configuration;}
    std::shared_ptr<MetricReporter>                 get_reporter() {return m_reporter;}

    int                                             get_interval_setting() {
        return m_configuration->get_integer(MetricConfig::MERTICS_REPORTER_INTERVAL, 1);
    }

    static std::vector<ReporterSetupPtr>            from_configuration(ConfigurationPtr configuration);
};
