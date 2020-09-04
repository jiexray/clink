/**
 * Simple factory which just wrap existed StreamOperator
 */
#pragma once
#include "StreamOperatorFactory.hpp"
#include "../functions/Function.hpp"
#include "SimpleUdfStreamOperatorFactory.hpp"
#include "AbstractUdfStreamOperator.hpp"

template <class OUT> class StreamOperatorParameters;

template <class OUT>
class SimpleStreamOperatorFactory : public StreamOperatorFactory<OUT>
{
private:
    std::shared_ptr<StreamOperator<OUT>>                        m_operator;
public:
    static std::shared_ptr<SimpleStreamOperatorFactory<OUT>>    of(std::shared_ptr<StreamOperator<OUT>> op);
    std::shared_ptr<StreamOperator<OUT>>                        create_stream_operator(std::shared_ptr<StreamOperatorParameters<OUT>> parameters) = 0;
};
