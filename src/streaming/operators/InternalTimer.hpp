#pragma once

/**
  Internal interface for in-flight timers.

  @param <K> Type of the keys to which timers are scoped.
  @param <N> Type of the namespace to which timers are scoped.
 */
template <class K, class N>
class InternalTimer
{
public:
    virtual long get_timestamp() = 0;

    virtual K& get_key() = 0;

    virtual N& get_namespace() = 0;
};