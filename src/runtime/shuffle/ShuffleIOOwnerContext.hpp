/**
 * Context of shuffle input/output owner used to create partitions or gates belonging to the owner.
 */
#pragma once

#include "MetricGroup.hpp"
#include <memory>

class ShuffleIOOwnerContext
{
private:
    typedef std::shared_ptr<MetricGroup>    MetricGroupPtr;
    std::string                     m_owner_name;
    int                             m_execution_id;
    MetricGroupPtr                  m_parent_group;
    MetricGroupPtr                  m_input_group;
    MetricGroupPtr                  m_output_group;
public:
    
    ShuffleIOOwnerContext(
        const std::string& owner_name, 
        int execution_id,
        MetricGroupPtr parent_group,
        MetricGroupPtr output_group,
        MetricGroupPtr input_group
        ):
        m_owner_name(owner_name),
        m_execution_id(execution_id),
        m_parent_group(parent_group),
        m_output_group(output_group),
        m_input_group(input_group) {}

    /* Properties */
    std::string                     get_owner_name() {return m_owner_name;}
    int                             get_execution_id() {return m_execution_id;}
    MetricGroupPtr                  get_parent_group() {return m_parent_group;}
    MetricGroupPtr                  get_output_group() {return m_output_group;}
    MetricGroupPtr                  get_input_group() {return m_input_group;}
};
