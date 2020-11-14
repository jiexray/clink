#pragma once
#include <string>
#include "TemplateHelper.hpp"

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
    typedef typename std::remove_pointer<T>::type* Tp;
    /** Name that uniquely identifies state created from this StateDescriptor. */
    std::string         m_name;
    Tp                  m_default_value;

    StateDescriptor(const std::string& name, Tp default_value): m_name(name), m_default_value(default_value) {}

    
public:
    ~StateDescriptor() {
        delete m_default_value;
    }

    enum Type {
        UNKNOWN,
        VALUE,
        LIST,
        REDUCING,
        FOLDING,
        AGGREGATING,
        MAP
    };

    virtual Type        get_type() = 0;
};

