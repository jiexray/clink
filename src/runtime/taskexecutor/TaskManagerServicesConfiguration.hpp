/**
 * Configuration for the task manager services such as the memory manager,
 * io manager and the metric registry.
 */
#pragma once
#include <string>
#include <iostream>
#include <memory>
#include "Configuration.hpp"

class TaskManagerServicesConfiguration
{
private:
    typedef std::shared_ptr<Configuration> ConfigurationPtr;
    typedef std::shared_ptr<TaskManagerServicesConfiguration> TaskManagerServicesConfigurationPtr;

    ConfigurationPtr                m_configuration;
    std::string                     m_task_manager_id;

    std::string                     m_external_address;
    int                             m_num_of_slots;

public:
    TaskManagerServicesConfiguration(ConfigurationPtr configuration,
                                     const std::string& task_manager_id,
                                     const std::string& external_address, 
                                     int num_of_slots):
                                     m_configuration(configuration),
                                     m_task_manager_id(task_manager_id),
                                     m_external_address(external_address),
                                     m_num_of_slots(num_of_slots) {}

    /* Properties */
    ConfigurationPtr                get_configuration() {return m_configuration;}

    std::string                     get_task_manager_id() {return m_task_manager_id;}

    std::string                     get_external_address() {return m_external_address;}

    int                             get_num_of_slots() {return m_num_of_slots;}

    static TaskManagerServicesConfigurationPtr from_configuration(ConfigurationPtr configuration, 
                                                                  const std::string& task_manager_id, 
                                                                  const std::string& external_address) {
        // TODO: extract configuration from configuration
        int num_of_slots = 10;

        return std::make_shared<TaskManagerServicesConfiguration>(
            configuration, 
            task_manager_id,
            external_address,
            num_of_slots);

    }
};


