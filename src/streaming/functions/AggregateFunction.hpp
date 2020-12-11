#pragma once
#include "TemplateHelper.hpp"
#include "Function.hpp"

/**
  The AggregateFunction is a flexible aggregation function.
 */
template <class IN, class ACC, class OUT>
class AggregateFunction: public Function
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::type ParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::const_type ConstParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::type ParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::const_type ConstParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::type ParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::const_type ConstParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::ret_ptr_type RetACC;
public:
    /**
      Create a new accumulator, starting a new aggregate.

      Note: the return value is located in heap, and the space must be released by caller!  

      @return A new accumulator, corresponding to an empty aggregate.
     */
    virtual ACC* create_accumulator() = 0;

    /**
      Adds the given input value to the given accumulator, returning the
      new accumulator value.

      Note: the return value is located in heap, and the space must be released by caller!  

      @param value The value to add
      @param accumulator The accumulator to add the value to
     */
    virtual ACC* add(const IN* value, const ACC* accumulator) = 0;

    /**
      Gets the result of the aggregation from the accumulator.

      Note: the return value is located in heap, and the space must be released by caller!  

      @param accumulator The accumulator of the aggregation.
      @return The final aggregation result.
     */
    virtual OUT* get_result(const ACC* accumulator) = 0;
};

