#include "OperatorMetricGroup.hpp"

OperatorMetricGroup::OperatorMetricGroup(MetricRegistryPtr registry, std::shared_ptr<TaskMetricGroup> parent, int operator_id, std::string operator_name):
    ComponentMetricGroup<TaskMetricGroup>(registry, ScopeFormats::get_instance()->get_operator_format()->format_scope(parent, operator_id, operator_name) , parent),
    m_operator_id(operator_id), m_operator_name(operator_name) {}
