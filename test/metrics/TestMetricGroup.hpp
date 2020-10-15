#pragma once
#include "TaskManagerMetricGroup.hpp"
#include "TaskManagerJobMetricGroup.hpp"
#include "TaskMetricGroup.hpp"
#include "OperatorMetricGroup.hpp"
#include "AbstractMetricGroup.hpp"
#include "MetricRegistryConfiguration.hpp"
#include "MetricRegistry.hpp"
#include "MetricReporter.hpp"
#include "ComponentMetricGroup.hpp"

#include "cxxtest/TestSuite.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>
#include <chrono>

class TestMetricGroup: public CxxTest::TestSuite
{
private: 
    typedef std::shared_ptr<MetricRegistry> MetricRegistryPtr;
    typedef std::shared_ptr<TaskManagerMetricGroup> TaskManagerMetricGroupPtr;
    typedef std::shared_ptr<TaskManagerJobMetricGroup> TaskManagerJobMetricGroupPtr;
    typedef std::shared_ptr<OperatorMetricGroup> OperatorMetricGroupPtr;
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;
    typedef std::shared_ptr<MetricRegistryConfiguration> MetricRegistryConfigurationPtr;
    typedef std::shared_ptr<ReporterSetup> ReporterSetupPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;
    typedef std::shared_ptr<Counter> CounterPtr;
    typedef std::shared_ptr<Meter> MeterPtr;
    typedef std::shared_ptr<ViewUpdater> ViewUpdaterPtr;

public:
    void testTaskManagerMetricGroupCreate( void ) {
        std::cout << "test testTaskManagerMetricGroupCreate()" << std::endl;
        MetricRegistryConfigurationPtr registry_config = MetricRegistryConfiguration::from_configuration();
        std::vector<ReporterSetupPtr> reportes;

        MetricRegistryPtr registry = std::make_shared<MetricRegistry>(registry_config, reportes);
        // std::shared_ptr<AbstractMetricGroup<int>> abstract_metric_group = std::make_shared<AbstractMetricGroup<int>>(registry,std::vector<std::string>{"test"}, nullptr);
        // std::shared_ptr<ComponentMetricGroup<int>> component_metric_group = std::make_shared<ComponentMetricGroup<int>>(registry,std::vector<std::string>{"test"}, nullptr);
        TaskManagerMetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, "host-1", "tm-1");
        TaskManagerJobMetricGroupPtr task_manager_job_metric_group = 
                                std::make_shared<TaskManagerJobMetricGroup>(registry, task_manager_metric_group, 0, "test-job"); // job-id, job-name
        // vertex-id, execution-id, task-name, subtask-index
        TaskMetricGroupPtr task_metric_group = std::make_shared<TaskMetricGroup>(registry, task_manager_job_metric_group, 0, 0, "test-task", 0); 
        // operator_id, operator_name
        OperatorMetricGroupPtr operator_metric_group = std::make_shared<OperatorMetricGroup>(registry, task_metric_group, 0, "test-operator");
        
        registry->shutdown();
    }

    void testAddGroup( void ){
        std::cout << "test testAddGroup()" << std::endl;
        MetricRegistryConfigurationPtr registry_config = MetricRegistryConfiguration::from_configuration();
        std::vector<ReporterSetupPtr> reportes;

        MetricRegistryPtr registry = std::make_shared<MetricRegistry>(registry_config, reportes);
        MetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, "host-1", "tm-1");

        MetricGroupPtr sub_group_in_task_manager = task_manager_metric_group->add_group("sub-group-1");
        std::vector<std::string>& sub_group_scope_components = sub_group_in_task_manager->get_scope_components();
        std::cout << "sub group of TaskManagerMetricGroup scope components: ";
        for (std::string s: sub_group_scope_components) {
            std::cout << s << ".";
        }
        std::cout << std::endl;

        MetricGroupPtr sub_sub_group_in_task_manager = sub_group_in_task_manager->add_group("sub-group-2");
        sub_group_scope_components = sub_sub_group_in_task_manager->get_scope_components();
        std::cout << "sub sub group of TaskManagerMetricGroup scope components: ";
        for (std::string s: sub_group_scope_components) {
            std::cout << s << ".";
        }
        std::cout << std::endl;

        registry->shutdown();
    }

    void testAddMetric( void ) {
        std::cout << "test testAddMetric()" << std::endl;
        MetricRegistryConfigurationPtr registry_config = MetricRegistryConfiguration::from_configuration();
        std::vector<ReporterSetupPtr> reportes;

        MetricRegistryPtr registry = std::make_shared<MetricRegistry>(registry_config, reportes);
        MetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, "host-1", "tm-1");

        CounterPtr counter = task_manager_metric_group->counter("test-counter");

        counter->inc();

        MeterPtr meter = task_manager_metric_group->meter("test-meter", std::make_shared<MeterView>(20));

        meter->mark_event();

        // task_manager_metric_group->get_metric_identifier();

        registry->shutdown();
    }

    // void testViewUpdater( void ) {
    //     std::cout << "test testViewUpdater()" << std::endl;
    //     boost::asio::io_service io_service;
    //     boost::asio::io_service::work work(io_service);
    //     boost::thread_group thr_grp;
    //     thr_grp.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));

    //     ViewUpdaterPtr view_updater = std::make_shared<ViewUpdater>(io_service);
    //     // io_service.run();

    //     std::this_thread::sleep_for(std::chrono::seconds(3));

    //     io_service.stop();
    //     thr_grp.join_all();
    // }

    void testMetricRegistry( void ) {
        std::cout << "test testMetricRegistry()" << std::endl;
        MetricRegistryConfigurationPtr registry_config = MetricRegistryConfiguration::from_configuration();
        std::vector<ReporterSetupPtr> reporters;

        MetricRegistryPtr registry = std::make_shared<MetricRegistry>(registry_config, reporters);

        MetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, "host-1", "tm-1");

        MeterPtr meter = task_manager_metric_group->meter("test-meter", std::make_shared<MeterView>(20));

        std::this_thread::sleep_for(std::chrono::seconds(3));

        registry->shutdown();
    }

    void testMetricReporter( void ) {
        std::cout << "test testMetricReporter()" << std::endl;
        MetricRegistryConfigurationPtr registry_config = MetricRegistryConfiguration::from_configuration();

        MetricRegistryPtr registry = std::make_shared<MetricRegistry>(registry_config, ReporterSetup::from_configuration(nullptr));

        MetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, "host-1", "tm-1");

        MeterPtr meter = task_manager_metric_group->meter("test-meter", std::make_shared<MeterView>(20));

        std::this_thread::sleep_for(std::chrono::seconds(5));
        meter->mark_event();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        meter->mark_event();
        std::this_thread::sleep_for(std::chrono::seconds(2));

        registry->shutdown();
    }
};

