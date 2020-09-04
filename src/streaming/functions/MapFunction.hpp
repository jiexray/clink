/**
 * Base interface for Map function.
 */
#pragma once
#include "Function.hpp"
#include <memory>

template <class T, class O>
class MapFunction: public Function
{
public:
    ~MapFunction() {}
    virtual std::shared_ptr<O>                  map(T& value) = 0;
    virtual char*                               serialize() {return nullptr;}
    virtual std::shared_ptr<MapFunction<T, O>>  deserialize() {return nullptr;}
};
