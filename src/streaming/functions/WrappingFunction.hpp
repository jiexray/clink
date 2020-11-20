#pragma once

#include "TemplateHelper.hpp"
#include "Function.hpp"

/**
  Wrapper around Function.
 */
template<class T>
class WrappingFunction
{
private:
    T& m_wrapped_function;
public:
    WrappingFunction(T& wrapped_function): m_wrapped_function(wrapped_function) {
        TemplateHelperUtil::CheckInherit<Function, T>::assert_inherit();
    }

    T& get_wrapped_function() {
        return m_wrapped_function;
    }

    virtual ~WrappingFunction() = default;
};
