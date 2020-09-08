/**
 * Container class for job information which is stored in the ExecutionGraph.
 */
#pragma once
#include <string>

class JobInformation
{
private:
    int                 m_job_id;
    std::string         m_job_name;
public:
    JobInformation(int job_id, std::string job_name): m_job_id(job_id), m_job_name(job_name){}

    /* Properties */
    int                 get_job_id() {return m_job_id;}
    std::string         get_job_name() {return m_job_name;}
};

