/**
 * This is used as the base class for operators that have a user-defined
 * function. This class handles the opening and closing of the user-defined functions
 */
#pragma once
#include "StreamOperator.hpp"
#include "Function.hpp"
#include "Output.hpp"
#include "ExecutionConfig.hpp"
#include <iostream>

template <class F, class OUT>
class AbstractUdfStreamOperator : virtual public StreamOperator<OUT>
{
private:
    // TODO: add context info
protected:
    std::shared_ptr<F>                              m_user_function;

    // Output of this operator, collect the produced data, it can be chaining output, result writer output
    std::shared_ptr<Output<OUT>>                    m_output;
public:
    AbstractUdfStreamOperator(std::shared_ptr<F> user_function):m_user_function(user_function){}
    
    virtual void                                    setup(std::shared_ptr<Output<OUT>> output) {
        this->m_output = output;
    }
    virtual void                                    close() {}
    virtual void                                    open() {}
};
