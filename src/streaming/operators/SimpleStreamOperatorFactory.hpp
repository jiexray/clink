/**
 * Simple factory which just wrap existed StreamOperator
 */
#pragma once
#include "StreamOperatorFactory.hpp"
#include "Function.hpp"
#include "SimpleUdfStreamOperatorFactory.hpp"
#include "AbstractUdfStreamOperator.hpp"

template <class OUT> class StreamOperatorParameters;
template <class OUT> class SimpleUdfStreamOperatorFactory;
template <class F, class OUT> class AbstractUdfStreamOperator;
template <class OUT> class StreamOperatorFactory;


template <class OUT>
class SimpleStreamOperatorFactory : public StreamOperatorFactory<OUT>
{
private:
    std::shared_ptr<StreamOperator<OUT>>                        m_operator;
public:
    static std::shared_ptr<SimpleStreamOperatorFactory<OUT>>    of(std::shared_ptr<StreamOperator<OUT>> op) {
        if (op == nullptr) {
            return nullptr;
        } else if (std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, OUT>>(op)) {
            // The operator is a sub-class of AbstractUdfStreamOperator
            // std::cout << "[DEBUG] SimpleStreamOperatorFactory<OUT>::of(): is AbstractUdfStreamOperator" << std::endl;
            std::shared_ptr<AbstractUdfStreamOperator<Function, OUT>> udf_operator = std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, OUT>>(op);
            return std::make_shared<SimpleUdfStreamOperatorFactory<OUT>>(udf_operator);
        }
        std::cout << "Unknown Stream Operator Type" << std::endl;
        throw std::runtime_error("Unknown Stream Operator Type");
        return nullptr;
    }

    std::shared_ptr<StreamOperator<OUT>>                        create_stream_operator(std::shared_ptr<StreamOperatorParameters<OUT>> parameters) = 0;
};
