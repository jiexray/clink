/**
 * Helper class to construct AbstractUdfStreamOperator. Wraps couple of internal parameters
 * to simplify for user construction of class extending AbstractUdfStreamOperator and to 
 * allow for backward compatible changes in the AbstractUdfStreamOperator.
 */

#pragma once
#include <memory>
#include "../task/StreamTask.hpp"
#include "StreamOperator.hpp"
#include "Output.hpp"

template<class OUT> class StreamTask;

template <class OUT>
class StreamOperatorParameters
{
private:
    std::shared_ptr<Output<OUT>>                            m_output;
    std::shared_ptr<StreamTask<OUT>>                        m_containing_task;
public:
    StreamOperatorParameters(std::shared_ptr<StreamTask<OUT>> containing_task, std::shared_ptr<Output<OUT>> output):
    m_containing_task(containing_task), m_output(output){}

    /* Properties */
    std::shared_ptr<Output<OUT>>                            get_output() {return m_output;}
    std::shared_ptr<StreamTask<OUT>>                        get_containing_task() {return m_containing_task;}
};

