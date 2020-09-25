/**
 * Class representing the operators in the streaming programs.
 */
#pragma once

#include <string>
#include <memory>
#include <vector>

class StreamNode
{
private:
    int                                                 m_id;
    int                                                 m_parallelism;
    std::string                                         m_operator_name;

    
public:
    StreamNode(int id, std::string operator_name) :
    m_id(id), m_operator_name(operator_name) {}

    /* Properties */
    std::string                                         get_operator_name() {return m_operator_name;}
    int                                                 get_id() {return m_id;}

    std::string                                         to_string() {return m_operator_name + "-" + std::to_string(m_id);}
};

