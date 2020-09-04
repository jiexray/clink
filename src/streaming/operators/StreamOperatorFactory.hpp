/**
 * A factory to create StreamOperator.
 */
#pragma once
#include "StreamOperator.hpp"
#include "StreamOperatorParameters.hpp"
#include <memory>

template <class OUT>
class StreamOperatorFactory
{
public:
    virtual std::shared_ptr<StreamOperator<OUT>>    create_stream_operator(std::shared_ptr<StreamOperatorParameters<OUT>> parameters) = 0;
};
