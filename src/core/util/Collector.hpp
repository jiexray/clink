/**
 * Collect a record and forwards it.
 */
#pragma once
#include <memory>

template <class T>
class Collector
{
public:
    virtual void collect(std::shared_ptr<T> record) = 0;
};
