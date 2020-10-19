/**
 * Utility clas to register pre-defined metric set.
 */
#pragma once
#include "TaskManagerMetricGroup.hpp"
#include "TaskManagerJobMetricGroup.hpp"
#include "TaskMetricGroup.hpp"
#include "OperatorMetricGroup.hpp"
#include "MetricGroup.hpp"
#include <memory>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

class MetricUtils
{
private:
    typedef std::shared_ptr<TaskManagerMetricGroup> TaskManagerMetricGroupPtr;
    typedef std::shared_ptr<TaskManagerJobMetricGroup> TaskManagerJobMetricGroupPtr;
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;
    typedef std::shared_ptr<OperatorMetricGroup> OperatorMetricGroupPtr;
    typedef std::shared_ptr<MetricRegistry> MetricRegistryPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;

    const static std::string METRIC_GROUP_STATUS_NAME;
    const static std::string METRIC_ACTOR_SYSTEM_NAME;

    static MetricGroupPtr create_and_instantiate_status_metric_group(MetricGroupPtr parent_metric_group) {
        MetricGroupPtr status_group = parent_metric_group->add_group(METRIC_GROUP_STATUS_NAME);
        instantiate_status_metrics(status_group);

        return status_group;
    }

    static void instantiate_status_metrics(MetricGroupPtr metric_group) {
        MetricGroupPtr system = metric_group->add_group("system");

        instantiate_memory_metrics(system->add_group("Memory"));
    }

    static void instantiate_memory_metrics(MetricGroupPtr metric_group) {
        metric_group->gauge("MemoryUsed", std::make_shared<AttributeGauge>("MemoryUsed"));
    }

public:
    static boost::tuple<TaskManagerMetricGroupPtr, MetricGroupPtr> instantiate_task_manager_metric_group(MetricRegistryPtr registry,
                                                                           const std::string& hostname,
                                                                           const std::string& task_manager_id) {
        TaskManagerMetricGroupPtr task_manager_metric_group = std::make_shared<TaskManagerMetricGroup>(registry, hostname, task_manager_id);

        MetricGroupPtr status_group = create_and_instantiate_status_metric_group(task_manager_metric_group);

        return boost::make_tuple(task_manager_metric_group, status_group);
    }

    class AttributeGauge: public Gauge {
    private:
        std::string m_attribute_name;
        int get_value_impl () {
            // TODO: implment memory usage dump
            return 1000;
        }
    public:
        friend struct GaugeVisitorImpl<AttributeGauge>;
        AttributeGauge(std::string attribute_name): Gauge(*(new GaugeVisitorImpl<AttributeGauge>(*this))) ,m_attribute_name(attribute_name) {}
    };
    
};

