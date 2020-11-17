#pragma once
#include "TemplateHelper.hpp"
#include "StateDescriptor.hpp"

/**
  This factory produces concrete internal state objects.

  @param <N> the type of the namespace
  @param <SV> the type of the stored state value
  @param <S> the type of the public API state
  @param <IS> the type of internal state
 */
template<class N, class SV, class S, class IS> 
class KeyedStateFactory
{
public:
    /**
      Creates and returns a new InternalKvState
      @param state_desc The StateDescriptor that contains the name of the state.      
     */
    // TODO: optimize the return type
    virtual IS* create_internal_state(const StateDescriptor<S, SV>& state_desc) = 0;
};

