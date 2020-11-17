#pragma once
#include <string>
#include "TemplateHelper.hpp"

enum StateDescriptorType {
    UNKNOWN,
    VALUE,
    LIST,
    REDUCING,
    FOLDING,
    AGGREGATING,
    MAP
};

/**
  Base class for state descriptor. A StateDescriptor is used for creating partitioned
  State in stateful operations.

  @param <S> The type of the State object create from this StateDecriptor.
  @param <T> The type of the value of the state object described by this StateDescriptor.
 */
template <class S, class T>
class StateDescriptor
{
protected:
    /** Name that uniquely identifies state created from this StateDescriptor. */
    typedef typename TemplateHelperUtil::ParamOptimize<T>::type ParamT;
    typedef typename TemplateHelperUtil::ParamOptimize<T>::const_type ConstParamT;

    std::string         m_name;
    T                   m_default_value;

    StateDescriptor(const std::string& name, ConstParamT default_value): m_name(name), m_default_value(default_value) {}

    
public:

    /* Properties */
    std::string get_name() const{
        return m_name;
    }

    ConstParamT get_default_value() const {
        return m_default_value;
    }

    virtual StateDescriptorType        get_type() = 0;
};

