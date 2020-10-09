#pragma once
#include "TypeSerializer.hpp"
#include "StringSerializer.hpp"
#include "DoubleSerializer.hpp"
#include "TupleSerializer.hpp"
#include "IntSerializer.hpp"
#include "TemplateHelper.hpp"

template <class T> class TypeSerializerFactory; 

template <class T> class TupleSerializer;

template <class T>
class TypeSerializerFactory {
public:
    static std::shared_ptr<TypeSerializer<T>>    of();
};

template <template <class, class> class T, class T1, class T2>
class TypeSerializerFactory<T<T1, T2>> {
public: 
    static std::shared_ptr<TypeSerializer<T<T1, T2>>>    of() {
        return std::make_shared<TupleSerializer<T<T1, T2>>>();
    }
};