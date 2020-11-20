#pragma once
#include "InternalTimer.hpp"

/**
  Interface for things that can be called by InternalTimerService.

  @param <K> Type of the keys to which timers are scoped.
  @param <N> Type of the namespace to which timers are scoped.
 */
template <class K, class N>
class Triggerable
{
public:
    virtual void on_event_time(InternalTimer<K, N>& timer) = 0

    virtual void on_processing_time(InternalTimer<K, N>& timer) = 0
};
