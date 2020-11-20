#pragma once

#include "AbstractUdfStreamOperator.hpp"
#include "InternalWindowFunction.hpp"
#include "OneInputStreamOperator.hpp"
#include "Triggerable.hpp"
#include "WindowAssigner.hpp"
#include "Trigger.hpp"
#include "StateDescriptor.hpp"
#include "InternalKvState.hpp"
#include <string>
#include <memory>

/**
  An operator that implements the logic for windowing based on a WindowAssigner and Trigger.

  @param <K> The type of key returned by the KeySelector
  @param <IN> The type of the incoming elements.
  @param <OUT> The type of the elements emitted by the InternalWindowFunction.
  @param <W> The type of Window that WindowAssigner assigns.
 */
template<class K, class IN, class ACC, class OUT, class W>
class WindowOperator: 
    public AbstractUdfStreamOperator<OUT, InternalWindowFunction<ACC, OUT, K, W>>,
    public OneInputStreamOperator<IN, OUT>,
    Triggerable<K, W> {
private:

    // -------------------------------------------------------------
    // Configuration values and user functions
    // -------------------------------------------------------------
    WindowAssigner<IN, W>& m_window_assigner;

    Trigger<IN, W>& m_trigger;


    // -------------------------------------------------------------
    // State that is not checkpointed
    // -------------------------------------------------------------

    /** The state in which the window contexts is stored. Each window is a name space */
    // TODO: implement InternalAppendingState
    // InternalKvState<K, >

    // -------------------------------------------------------------
    // State that needs to be checkpointed
    // -------------------------------------------------------------

public:
    WindowOperator();
    ~WindowOperator();
};
