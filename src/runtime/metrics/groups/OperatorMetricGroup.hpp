/**
 * Special MetricGroup representing an Operator.
 */
#pragma once
#include "ComponentMetricGroup.hpp"
#include "TaskMetricGroup.hpp"
#include "OperatorIOMetricGroup.hpp"

class TaskMetricGroup;
class MetricRegistry;
class OperatorIOMetricGroup;

class OperatorMetricGroup: public ComponentMetricGroup<TaskMetricGroup>, public inheritable_enable_shared_from_this<OperatorMetricGroup>
{
private:
    typedef std::shared_ptr<MetricRegistry> MetricRegistryPtr;

    std::string                                 m_operator_id;
    std::string                                 m_operator_name;

    std::shared_ptr<OperatorIOMetricGroup>      m_io_metrics;
public:
    OperatorMetricGroup(MetricRegistryPtr registry, std::shared_ptr<TaskMetricGroup> parent, const std::string& operator_id, const std::string& operator_name);

    std::shared_ptr<TaskMetricGroup> get_parent() {return m_parent;}

    std::shared_ptr<OperatorIOMetricGroup> get_IO_metric_group() {
        if (m_io_metrics == nullptr) {
            m_io_metrics = std::make_shared<OperatorIOMetricGroup>(inheritable_enable_shared_from_this<OperatorMetricGroup>::shared_from_this());
        }
        return m_io_metrics;
    }

    // -------------------------------------------------------------
    //  Component Metric Group Specifics
    // -------------------------------------------------------------

    std::string get_group_name() {return "operator";}

    void close() override {
        std::unique_lock<std::mutex> lock(m_global_mtx);
        ComponentMetricGroup<TaskMetricGroup>::close();
    }
};

