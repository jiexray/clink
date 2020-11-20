#pragma once
#include "InternalKvState.hpp"
#include "AppendingState.hpp"
#include "TemplateHelper.hpp"

/**
  The peer to the ApendingState in the internal state type hierarchy.

  @param <K> The type of key the state is associated to
  @param <N> The type of the namespace
  @param <IN> The type of elements added to the state
  @param <SV> The type of elements in the state
  @param <OUT> The type of the resulting element in the state
 */
template <class K, class N, class IN, class SV, class OUT>
class InternalAppendingState: 
    virtual public InternalKvState<K, N, SV> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::const_type ConstParamSV;
public:
    /**
     Get internally stored value.
     */
    virtual ParamSV get_internal() = 0;

    /**
      Update internally stored value.
     */
    virtual void update_internal(ParamSV value_to_store) = 0;
};
