/**
 * This class is the executable entry point for the task manager.
 * It construct the related componenets (network, IO mananger, memory manager) and starts them.
 */
#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "LoggerFactory.hpp"
#include "MetricRegistry.hpp"
#include "TaskExecutor.hpp"
#include "Configuration.hpp"
#include "MetricUtils.hpp"
#include "TaskManagerServices.hpp"

class TaskExecutorRunner
{
private:
    typedef std::shared_ptr<MetricRegistry> MetricRegistryPtr;
    typedef std::shared_ptr<TaskExecutor> TaskExecutorPtr;
    typedef std::shared_ptr<Configuration> ConfigurationPtr;
    typedef std::shared_ptr<spdlog::logger> LoggerPtr;
    typedef std::shared_ptr<TaskManagerMetricGroup> TaskManagerMetricGroupPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;
    typedef std::shared_ptr<TaskManagerServices> TaskManagerServicesPtr;
    typedef std::shared_ptr<TaskManagerServicesConfiguration> TaskManagerServicesConfigurationPtr;

    std::string                             m_task_manager_id;

    ConfigurationPtr                        m_configuration;
    MetricRegistryPtr                       m_registry;
    TaskExecutorPtr                         m_task_manager;
    bool                                    m_shutdown;

    static LoggerPtr                        m_logger;
public:
    TaskExecutorRunner(ConfigurationPtr configuration, const std::string& task_manager_id):
    m_task_manager_id(task_manager_id), m_configuration(configuration){

        m_registry = std::make_shared<MetricRegistry>(
                                                MetricRegistryConfiguration::from_configuration(configuration), 
                                                ReporterSetup::from_configuration(configuration));
        
        m_task_manager = start_task_manager(configuration, task_manager_id, m_registry);
        m_shutdown = false;
    }

    /* Properties */
    TaskExecutorPtr                         get_task_executor() {return m_task_manager;}

    static TaskExecutorPtr start_task_manager(ConfigurationPtr configuration, 
                                              const std::string& task_manager_id, 
                                              MetricRegistryPtr registry) {
        SPDLOG_LOGGER_INFO(m_logger, "starting TaskManager with ID: {}", task_manager_id);

        std::string hostname = "localhost";

        boost::tuple<TaskManagerMetricGroupPtr, MetricGroupPtr> task_manager_metric_group = MetricUtils::instantiate_task_manager_metric_group(registry, hostname, task_manager_id);

        TaskManagerServicesConfigurationPtr task_manager_services_configuration = TaskManagerServicesConfiguration::from_configuration(configuration, task_manager_id, hostname);

        TaskManagerServicesPtr task_manager_services = TaskManagerServices::from_configuration(task_manager_services_configuration, task_manager_metric_group.get<1>());

        return std::make_shared<TaskExecutor>(task_manager_services, task_manager_metric_group.get<0>());

    }
};

