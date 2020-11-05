/**
 * Delagate for tuple deserializtion.
 */
#pragma once
#include <memory>
#include <typeinfo>
#include <vector>
#include <functional>
#include "IOReadableWritable.hpp"
#include "IntValue.hpp"
#include "StringValue.hpp"
#include "DoubleValue.hpp"
#include "TupleUtils.hpp"
#include "DeserializationDelegate.hpp"

template <class T>
class TupleDeserializationDelegate;

template<template <class, class> class T, class T0, class T1>
class TupleDeserializationDelegate<T<T0, T1>>: public IOReadableWritable
{
private:
    /* members for tupleize read */
    int                                     m_num_of_values;
    // (const std::type_info&)*                m_value_types;
    std::shared_ptr<IOReadableWritable>*    m_real_values;

public:
    TupleDeserializationDelegate() {
        m_real_values = new std::shared_ptr<IOReadableWritable>[2];
        m_real_values[0] = std::make_shared<DeserializationDelegate<T0>>();
        m_real_values[1] = std::make_shared<DeserializationDelegate<T1>>();
    }

    ~TupleDeserializationDelegate() {
        delete[] m_real_values;
        m_real_values = nullptr;
    }

    /* Properties */
    int                                     get_num_of_values() override {return 2;}

    /* Implement functions in IOReadableWritable */
    std::shared_ptr<T<T0, T1>>                  get_instance() {
        return std::make_shared<T<T0, T1>>(
            std::dynamic_pointer_cast<DeserializationDelegate<T0>>(m_real_values[0])->get_instance(),
            std::dynamic_pointer_cast<DeserializationDelegate<T1>>(m_real_values[1])->get_instance());
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer) {
        throw std::runtime_error("No use");
    }

    std::shared_ptr<IOReadableWritable>     get_field(int pos) {
        return m_real_values[pos];
    }
};

