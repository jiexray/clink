/**
 * Interface for a metric registry.
 */
#pragma once

#include "Metric.hpp"
#include "MeterView.hpp"
// #include "AbstractMetricGroup.hpp"
#include "ViewUpdater.hpp"
#include "MetricReporter.hpp"
#include "ReporterSetup.hpp"
#include "ProxyMetricGroup.hpp"
#include "LoggerFactory.hpp"
#include "MetricRegistryConfiguration.hpp"
#include <memory>
#include <vector>
#include <mutex>

class MetricRegistry
{
private:
    typedef std::shared_ptr<ReporterSetup>          ReporterSetupPtr;
    typedef std::shared_ptr<MetricReporter>         MetricReporterPtr;
    typedef std::shared_ptr<ViewUpdater>            ViewUpdaterPtr;
    typedef std::shared_ptr<MetricRegistryConfiguration>
                                                    MetricRegistryConfigurationPtr; 

    ViewUpdaterPtr                                  m_view_updater;
    std::vector<MetricReporterPtr>                  m_reporters;
    std::mutex                                      m_register_mtx;
    char                                            m_global_delimiter;
    bool                                            m_shutdown;

    static std::shared_ptr<spdlog::logger>          m_logger;
    
public:
    // MetricRegistry(MetricRegistryConfigurationPtr config, const std::vector<ReporterSetupPtr>& reporter_configurations);
    MetricRegistry(MetricRegistryConfigurationPtr config, const std::vector<ReporterSetupPtr>& reporter_configurations) {
        if (!reporter_configurations.empty()) {
            for (ReporterSetupPtr reporter_setup: reporter_configurations) {
                std::string named_reporter = reporter_setup->get_name();
                MetricReporterPtr reporter_instance = reporter_setup->get_reporter();
                
                // TODO: Add reporter to the Timer

                m_reporters.push_back(reporter_instance);
            }
        }
        m_global_delimiter = config->get_delimiter();
    }

    /* Properties */
    inline int get_number_reporters() { return (int)m_reporters.size(); }

    inline char get_delimiter() {return m_global_delimiter;}

    // -------------------------------------------------------------
    //  Metrics (de)registration
    // -------------------------------------------------------------

    inline void register_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) {
        std::unique_lock<std::mutex> lock(m_register_mtx);
        if(m_shutdown) {
            SPDLOG_LOGGER_WARN(m_logger, "Cannot register metric, because the MetricRegistry has already been shut down.");
            return;
        }

        // notify reporters
        if (!m_reporters.empty()) {
            for (MetricReporterPtr reporter: m_reporters) {
                std::shared_ptr<ProxyMetricGroup<MetricGroup>> proxy_metric_group = std::make_shared<ProxyMetricGroup<MetricGroup>>(group);
                reporter->notify_of_added_metric(metric, metric_name, proxy_metric_group);
            }
        }

        if (std::dynamic_pointer_cast<MeterView>(metric) != nullptr) {
            // metric is a view, register it to view_updater
            if (m_view_updater == nullptr) {
                m_view_updater = std::make_shared<ViewUpdater>();
            }
            m_view_updater->notify_of_add_view(std::dynamic_pointer_cast<MeterView>(metric));
        }
    }

    inline void unregister_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) {
        std::unique_lock<std::mutex> lock(m_register_mtx);
        if(m_shutdown) {
            SPDLOG_LOGGER_WARN(m_logger, "Cannot register metric, because the MetricRegistry has already been shut down.");
            return;
        }

        // notify reporters
        if (!m_reporters.empty()) {
            for (MetricReporterPtr reporter: m_reporters) {
                std::shared_ptr<ProxyMetricGroup<MetricGroup>> proxy_metric_group = std::make_shared<ProxyMetricGroup<MetricGroup>>(group);
                reporter->notify_of_removed_metric(metric, metric_name, proxy_metric_group);
            }
        }

        if (std::dynamic_pointer_cast<MeterView>(metric) != nullptr) {
            // metric is a view, register it to view_updater
            if (m_view_updater != nullptr) {
                m_view_updater->notify_of_remove_view(std::dynamic_pointer_cast<MeterView>(metric));
            }
        }
    }
};