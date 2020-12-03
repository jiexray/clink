#pragma once
#include "State.hpp"
#include "TemplateHelper.hpp"

/**
  Base interface for partitioned state that supports adding elements and inspecting the current
  state. Elements can either be kept in a buffer (list-like) or aggregated into one value.

  @param <IN> Type of the value that can be added to the state.
  @param <OUT> Type of the value that can be retrieved from the state.
 */
template <class IN, class OUT>
class AppendingState: public State {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::type ParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::type ParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::const_type ConstParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::const_type ConstParamOUT;
public:
    /**
      Returns the current value for the state. When the state is not
	  partitioned the returned value is the same for all inputs in a given
	  operator instance. If state partitioning is applied, the value returned
	  depends on the current operator input, as the operator maintains an
	  independent state for each partition.

      @return The operator state value corresponding to the current input or null
      if the state is empty.
     */
    virtual ConstParamOUT get() = 0;

    /**
      Updates the operator state accessible by {@link #get()} by adding the given value
	  to the list of values. The next time {@link #get()} is called (for the same state
	  partition) the returned state will represent the updated list.
     */
    virtual void add(ConstParamIN value) = 0;
};

