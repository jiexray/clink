#pragma once

#include "TemplateHelper.hpp"
#include "Function.hpp"

/**
  Wrapper around Function.
 */
template<class T>
class WrappingFunction
{
public:
    T& m_wrapped_function;

    WrappingFunction(T& wrapped_function): m_wrapped_function(wrapped_function) {
        TemplateHelperUtil::CheckInherit<Function, T>::assert_inherit();
    }

    ~WrappingFunction() {
        delete &m_wrapped_function;
    }

    T& get_wrapped_function() {
        return m_wrapped_function;
    }

};
