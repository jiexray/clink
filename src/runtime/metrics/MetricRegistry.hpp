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
#include "Scheduled.hpp"
#include "Scheduler.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <boost/thread.hpp>
#include <functional>

class MetricRegistry
{
private:
    typedef Scheduler::PeriodicScheduler            PeriodicScheduler;
    typedef std::shared_ptr<PeriodicScheduler>      PeriodicSchedulerPtr;
    typedef std::shared_ptr<MetricRegistry>         MetricRegistryPtr;
    typedef std::shared_ptr<Scheduled>              ScheduledPtr;
    typedef std::shared_ptr<boost::asio::deadline_timer> TimerPtr;
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

    /* Periodical scheduler */
    PeriodicSchedulerPtr                            m_scheduler;
    // boost::thread                                   m_executor;
public:
    // MetricRegistry(MetricRegistryConfigurationPtr config, const std::vector<ReporterSetupPtr>& reporter_configurations);
    MetricRegistry(MetricRegistryConfigurationPtr config, const std::vector<ReporterSetupPtr>& reporter_configurations) {
        // start io_service for view updater
        m_scheduler = std::make_shared<PeriodicScheduler>();
        m_scheduler->run();
        // m_executor = boost::thread(boost::bind(&PeriodicScheduler::run, m_scheduler));

        if (!reporter_configurations.empty()) {
            int reporter_idx = 0;
            for (ReporterSetupPtr reporter_setup: reporter_configurations) {
                std::string named_reporter = reporter_setup->get_name();
                MetricReporterPtr reporter_instance = reporter_setup->get_reporter();
                
                // Add reporter to the Timer
                int period = reporter_setup->get_interval_setting();
                if (std::dynamic_pointer_cast<LoggerReporter>(reporter_instance) != nullptr) {
                    m_scheduler->add_periodic_task("reporter-timer", boost::bind(&Scheduled::report, std::dynamic_pointer_cast<LoggerReporter>(reporter_instance)), period);
                } else {
                    throw std::invalid_argument("Reporter is not a Scheduled instance");
                }

                m_reporters.push_back(reporter_instance);
                reporter_idx++;
            }
        }
        m_global_delimiter = config->get_delimiter();
        m_view_updater = nullptr;
        m_is_shutdown = false; 
    }

    /* Properties */
    int get_number_reporters() { return (int)m_reporters.size(); }

    char get_delimiter() {return m_global_delimiter;}

    // -------------------------------------------------------------
    //  Metrics (de)registration
    // -------------------------------------------------------------

    void register_metric(std::shared_ptr<Metric> metric, const std::string& metric_name, std::shared_ptr<MetricGroup> group) {
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
                m_view_updater = std::make_shared<ViewUpdater>(m_scheduler);
            }
            m_view_updater->notify_of_add_view(std::dynamic_pointer_cast<MeterView>(metric));
        }
    }

    void unregister_metric(std::shared_ptr<Metric> metric, const std::string& metric_name, std::shared_ptr<MetricGroup> group) {
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
            m_scheduler->stop();
            // m_executor.join();
        }
    }
};