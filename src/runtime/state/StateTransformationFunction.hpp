#pragma once
#include "TemplateHelper.hpp"

/**
  Interface for a binary function that is used for push-down of state transformation into state backends. The
  function takes as inputs the old state and an element. From those inputs, the function computes the new state.
 */
template <class S, class T>
class StateTransformationFunction
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<T>::type ParamT;
    typedef typename TemplateHelperUtil::ParamOptimize<T>::const_type ConstParamT;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::ret_ptr_type RetS;
public:
    /**
      Binary function that applies a given value to the given old state to compute the new state.

      @param previous_state The previous state that is the basis for the transformation, can be nullptr.
      @param value The value that the implementation applies to the old state to obtain the new state.
      @return the new state, computed by applying the given transformation function.
     */
    virtual S* apply(S* previous_state, ConstParamT value) = 0;
};
