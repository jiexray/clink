/**
 * Base interface for Map function.
 */
#pragma once
#include "Function.hpp"
#include <memory>
#include <type_traits>
#include "TemplateHelper.hpp"

template <class T, class O>
class MapFunction: public Function
{
public:
    ~MapFunction() {}

    /**
      Take care of the return value, it must be freed!
     */
    virtual O*                                  map(T* value) = 0;
    virtual char*                               serialize() {return nullptr;}
    virtual std::shared_ptr<MapFunction<T, O>>  deserialize() {return nullptr;}
};
