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
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>

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
    bool                                            m_is_shutdown;

    static std::shared_ptr<spdlog::logger>          m_logger;

    /* Boost */
    boost::asio::io_service                         m_io_service;
    // for io_service not stop
    boost::asio::io_service::work*                  m_io_service_work;
    boost::thread                                   m_io_service_executor;
    
    
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
        m_view_updater = nullptr;
        m_is_shutdown = false;

        // start io_service for view updater
        m_io_service_work = new boost::asio::io_service::work(m_io_service);
        m_io_service_executor = boost::thread(boost::bind(&boost::asio::io_service::run, &m_io_service));
    }

    /* Properties */
    int get_number_reporters() { return (int)m_reporters.size(); }

    char get_delimiter() {return m_global_delimiter;}

    // -------------------------------------------------------------
    //  Metrics (de)registration
    // -------------------------------------------------------------

    void register_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) {
        std::unique_lock<std::mutex> lock(m_register_mtx);
        if(m_is_shutdown) {
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
                m_view_updater = std::make_shared<ViewUpdater>(m_io_service);
            }
            m_view_updater->notify_of_add_view(std::dynamic_pointer_cast<MeterView>(metric));
        }
    }

    void unregister_metric(std::shared_ptr<Metric> metric, std::string metric_name, std::shared_ptr<MetricGroup> group) {
        std::unique_lock<std::mutex> lock(m_register_mtx);
        if(m_is_shutdown) {
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

    void shutdown() {
        std::unique_lock<std::mutex> lock(m_register_mtx);
        if (!m_is_shutdown) {
            m_is_shutdown = true;

            for (MetricReporterPtr reporter: m_reporters) {
                reporter->close();
            }
            m_reporters.clear();

            // stop io_service, finish its executor
            m_io_service.stop();
            delete m_io_service_work;
            m_io_service_executor.join();
        }
    }
};