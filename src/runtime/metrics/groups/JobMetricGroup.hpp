/**
 * Special abstract MetricGroup representing everything belonging to a specific job.
 */
#pragma once
#include "ComponentMetricGroup.hpp"
#include <iostream>

template <typename PC>
class JobMetricGroup : public ComponentMetricGroup<PC>
{
protected:
    int             m_job_id;
    std::string     m_job_name;
public:
    JobMetricGroup(std::shared_ptr<MetricRegistry> registry, std::shared_ptr<PC> parent, int job_id, const std::string job_name, const std::vector<std::string>& scope):
    ComponentMetricGroup<PC>(registry, scope, parent), m_job_id(job_id), m_job_name(job_name){}

    /* properties */
    int             get_job_id() {return m_job_id;}
    std::string     get_job_name() {return m_job_name;}

    /* Component Metric Group Specific */
protected: 
    virtual void put_variables(std::map<std::string, std::string>& variables) override {
        variables.insert(std::make_pair(ScopeFormat::SCOPE_JOB_ID, std::to_string(m_job_id)));
        variables.insert(std::make_pair(ScopeFormat::SCOPE_JOB_NAME, m_job_name));
    }

    virtual std::string get_group_name() {return "job";}
};

