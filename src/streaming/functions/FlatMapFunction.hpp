/**
 * Base interface for flatMap functions. FlatMap functions take elements and transform them
 * into zero, one, or more elements.
 */
#pragma once
#include "Function.hpp"
#include "Collector.hpp"
#include <memory>

template <class T, class O>
class FlatMapFunction : public Function
{
public:
    virtual void                                    flat_map(std::shared_ptr<T> value, 
                                                            std::shared_ptr<Collector<O>> collector) = 0;
    virtual char*                                   serialize() {return nullptr;}
    virtual std::shared_ptr<FlatMapFunction<T, O>>  deserialize() {return nullptr;}
};
