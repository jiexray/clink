/**
 * A Counter is a Metric that measures a count.
 */
#pragma once
#include "Metric.hpp"

class Counter: public Metric
{
public:
    virtual void inc() = 0;
    virtual void inc(long n) = 0;
    virtual void dec() = 0;
    virtual void dec(long n) = 0;

    virtual long get_count() = 0;
};
