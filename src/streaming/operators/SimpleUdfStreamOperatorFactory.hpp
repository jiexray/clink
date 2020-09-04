/**
 * Udf stream opeator factory which just wrap existed AbstractUdfStreamOperator.
 */
#pragma once
#include "SimpleStreamOperatorFactory.hpp"
#include "AbstractUdfStreamOperator.hpp"

template <class OUT> class SimpleStreamOperatorFactory;
template <class OUT> class StreamOperatorParameters;
template <class F, class OUT> class AbstractUdfStreamOperator;

template <class OUT>
class SimpleUdfStreamOperatorFactory final : public SimpleStreamOperatorFactory<OUT>
{
private:
    std::shared_ptr<AbstractUdfStreamOperator<Function, OUT>> m_operator;
public:
    SimpleUdfStreamOperatorFactory(std::shared_ptr<AbstractUdfStreamOperator<Function, OUT>> op) {
        this->m_operator = op;
    }
    std::shared_ptr<StreamOperator<OUT>> create_stream_operator(std::shared_ptr<StreamOperatorParameters<OUT>> parameters) {
        if (std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, OUT>>(this->m_operator).get() != nullptr) {
            // The operator is a sub-class of AbstractUdfStreamOperator
            this->m_operator->setup(parameters->get_containing_task(), parameters->get_output());
        }
        // std::cout << "[debug] before setup()" << std::endl;
        // this->m_operator->setup(parameters->get_containing_task(), parameters->get_output());
        return this->m_operator;
    }

    /* Properties */
    std::shared_ptr<AbstractUdfStreamOperator<Function, OUT>> get_operator() {return m_operator;}
};


