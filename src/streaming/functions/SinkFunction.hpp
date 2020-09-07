/**
 * Interface for implementing user defined sink functionality.
 */
#pragma once
#include "Function.hpp"

template <class IN>
class SinkFunction : public Function
{
public:
    virtual void                                        invoke(IN &value) = 0;
    virtual char*                                       serialize() = 0;
    virtual std::shared_ptr<SinkFunction<IN>>           deserialize() = 0;
};

