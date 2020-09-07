/**
 * Basic interface for stream operators.
 */
#pragma once


template <class OUT>
class StreamOperator
{
public:
    // virtual StreamOperator() = default;
    virtual ~StreamOperator() = default;
    // virtual void setup(std::shared_ptr<StreamTask<OUT>> containingTask, 
    //                     std::shared_ptr<Output<OUT>> output) = 0;
    // virtual void open() = 0;
    // virtual void print() = 0;
};
