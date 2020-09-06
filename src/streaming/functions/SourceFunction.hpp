/**
 * Basic interface for all stream data sources in Flink. 
 */
#pragma once
#include "Function.hpp"
#include <memory>

template <class T> class SourceContext;

// Note: class IN is no-use, just a placeholder
template <class OUT>
class SourceFunction : public Function
{
public:
    virtual void                                        run(std::shared_ptr<SourceContext<OUT>> ctx) = 0;
    virtual char*                                       serialize() {return nullptr;}
    virtual std::shared_ptr<SourceFunction<OUT>>        deserialize() {return nullptr;}
};

template <class OUT>
class SourceContext
{
public:
    virtual void collect(std::shared_ptr<OUT> element) = 0;
};



