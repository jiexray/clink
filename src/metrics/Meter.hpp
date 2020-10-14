/**
 * Metric for measuring throughput.
 */
#pragma once
#include "Metric.hpp"

class Meter: public Metric
{
public:
    virtual void    mark_event() = 0;
    virtual void    mark_event(long n) = 0;
    virtual double  get_rate() = 0;
    virtual long    get_count() = 0;
};
