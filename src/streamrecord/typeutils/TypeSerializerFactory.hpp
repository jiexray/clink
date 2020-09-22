#pragma once
#include "TypeSerializer.hpp"
#include "StringSerializer.hpp"
#include "DoubleSerializer.hpp"
#include "TupleSerializer.hpp"
#include "IntSerializer.hpp"


template <class T>
class TypeSerializerFactory {
public:
    static std::shared_ptr<TypeSerializer<T>>    of();
};

// template<> std::shared_ptr<TypeSerializer<std::string>> TypeSerializerFactory<std::string>::of();

// template<> std::shared_ptr<TypeSerializer<double>> TypeSerializerFactory<double>::of();