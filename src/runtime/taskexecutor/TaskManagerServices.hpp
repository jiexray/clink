/**
 * Container for TaskExecutor services such as MemoryManager, IOManager, ShuffleEnvironment.
 */
#pragma once
#include "ShuffleEnvironment.hpp"
#include "TaskSlotTable.hpp"
#include "Configuration.hpp"
#include "TaskManagerServicesConfiguration.hpp"
#include "MetricGroup.hpp"
#include <memory>
#include <string>
#include <iostream>

class TaskManagerServices
{
private:
    typedef std::shared_ptr<ShuffleEnvironment> ShuffleEnvironmentPtr;
    typedef std::shared_ptr<TaskSlotTable> TaskSlotTablePtr;
    typedef std::shared_ptr<TaskManagerServices> TaskManagerServicesPtr;
    typedef std::shared_ptr<Configuration> ConfigurationPtr;
    typedef std::shared_ptr<TaskManagerServicesConfiguration> TaskManagerServicesConfigurationPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;
    
    ShuffleEnvironmentPtr           m_shuffle_environment;
    TaskSlotTablePtr                m_task_slot_table;
public:
    /* Properties */

    ShuffleEnvironmentPtr           get_shuffle_environment() {return m_shuffle_environment;}
    TaskSlotTablePtr                get_task_slot_table() {return m_task_slot_table;}

    TaskManagerServices(ShuffleEnvironmentPtr shuffle_environment, TaskSlotTablePtr task_slot_table):
    m_shuffle_environment(shuffle_environment), m_task_slot_table(task_slot_table) {}


    void shutdown() {
        // TODO: shutdown all services
    }

    static TaskManagerServicesPtr from_configuration(
        TaskManagerServicesConfigurationPtr configuration,
        MetricGroupPtr metric_group) {
        // create shuffle_environment
        ShuffleEnvironmentPtr shuffle_environment = create_shuffle_environment(configuration, metric_group);

        // create task_slot_table
        TaskSlotTablePtr task_slot_table = create_task_slot_table(configuration->get_num_of_slots());

        return std::make_shared<TaskManagerServices>(shuffle_environment, task_slot_table);
    }

    static ShuffleEnvironmentPtr create_shuffle_environment(TaskManagerServicesConfigurationPtr task_manager_services_configuration, MetricGroupPtr task_manager_metric_group) {
        // TODO: support shuffle with network
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();
        std::shared_ptr<InputGateFactory> input_gate_factory = std::make_shared<InputGateFactory>(result_partition_manager);
        std::shared_ptr<ResultPartitionFactory> result_partition_factory = std::make_shared<ResultPartitionFactory>(result_partition_manager);

        /* create ShuffleEnvironment */
        return std::make_shared<LocalShuffleEnvironment>(result_partition_manager, result_partition_factory, input_gate_factory);
    }

    static TaskSlotTablePtr create_task_slot_table(int num_of_slots) {
        /* create TaskSlotTable */
        return std::make_shared<TaskSlotTable>(num_of_slots);
    }

};

