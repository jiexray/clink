/**
 * Class representing the operators in the streaming programs.
 */
#pragma once
#include "../operators/StreamOperatorFactory.hpp"

#include <string>
#include <memory>
#include <vector>

template <class OUT> class StreamOperatorFactory;

template <class OUT>
class StreamNode
{
private:
    int                                                 m_id;
    int                                                 m_parallelism;
    std::string                                         m_operator_name;

    std::shared_ptr<StreamOperatorFactory<OUT>>         m_operator_factory;
    
public:
    StreamNode(int id, std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory, std::string operator_name):
    m_id(id), m_operator_name(operator_name), m_operator_factory(operator_factory) {}

    /* Properties */
    std::string                                         get_operator_name() {return m_operator_name;}
    std::shared_ptr<StreamOperatorFactory<OUT>>         get_operator_factory() {return m_operator_factory;}
    int                                                 get_id() {return m_id;}

    std::string                                         to_string() {return m_operator_name + "-" + std::to_string(m_id);}
};

