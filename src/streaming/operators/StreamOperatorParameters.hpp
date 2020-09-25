/**
 * Helper class to construct AbstractUdfStreamOperator. Wraps couple of internal parameters
 * to simplify for user construction of class extending AbstractUdfStreamOperator and to 
 * allow for backward compatible changes in the AbstractUdfStreamOperator.
 */

#pragma once
#include <memory>
#include "StreamOperator.hpp"
#include "Output.hpp"


template <class OUT>
class StreamOperatorParameters
{
private:
    std::shared_ptr<Output<OUT>>                            m_output;
public:
    StreamOperatorParameters(std::shared_ptr<Output<OUT>> output): m_output(output){}

    /* Properties */
    std::shared_ptr<Output<OUT>>                            get_output() {return m_output;}
};

