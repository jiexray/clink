/**
 * An edge in the streaming topology. An interface class
 */
#pragma once
#include "../partitioner/StreamPartitioner.hpp"
#include "StreamNode.hpp"
#include <memory>

template <class T>
class StreamEdge
{
private:
    private:
    int                                         m_source_id;
    int                                         m_target_id;
    std::string                                 m_edge_id;
    std::shared_ptr<StreamPartitioner<T>>       m_output_partitioner;

    std::string                                 m_source_operator_name;
    std::string                                 m_target_operator_name;    
public:
    StreamEdge(std::shared_ptr<StreamNode<T>> source_vertex, std::shared_ptr<StreamNode<T>> target_vertex, 
                std::shared_ptr<StreamPartitioner<T>> output_partitioner) {
        m_source_id                     = source_vertex->get_id();
        m_target_id                     = target_vertex->get_id();

        this->m_output_partitioner      = output_partitioner;

        m_source_operator_name          = source_vertex->get_operator_name();
        m_target_operator_name          = target_vertex->get_operator_name();
        m_edge_id                       = source_vertex->to_string() + "_" + target_vertex->to_string() + "_" + this->m_output_partitioner->to_string();
    }

    StreamEdge(int source_id, int target_id, std::string source_operator_name, std::string target_operator_name, std::shared_ptr<StreamPartitioner<T>> output_partitioner):
    m_source_id(source_id), m_target_id(target_id), m_source_operator_name(source_operator_name), m_target_operator_name(target_operator_name),
    m_output_partitioner(output_partitioner) {
        m_edge_id = source_operator_name + "-" + std::to_string(source_id) + "_" + target_operator_name + "-" + std::to_string(target_id) + 
        "_" + this->m_output_partitioner->to_string();
    }
    /* Properties */
    std::shared_ptr<StreamPartitioner<T>>       get_output_partitioner() {return m_output_partitioner;}
    int                                         get_source_id() {return m_source_id;}
    std::string                                 get_source_operator_name() {return m_source_operator_name;}
    int                                         get_target_id() {return m_target_id;}
    std::string                                 get_target_operator_name() {return m_target_operator_name;}
    std::string                                 get_edge_id() {return m_edge_id;}
};


