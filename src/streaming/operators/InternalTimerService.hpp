#pragma once
#include "TemplateHelper.hpp"

/**
  Interface for working with time and timers.

  @param <N> Type of the namespace to which timers are scoped.
 */
template <class N>
class InternalTimerService
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
public:
    /**
      Returns the current proessing time.
     */
    virtual long current_processing_time() = 0;

    /**
      Return the current event-time watermark.
     */
    virtual long current_watermark() = 0;

    /**
      Registers a timer to be fired when processing time passes the given time. The namespace
      you pass here will be provided when the timer fire.
     */
    virtual void register_processing_time_timer(ConstParamN ns, long time) = 0;

    /**
      Deletes the timer for the given key and namespace.
     */
    virtual void delete_processing_time_timer(ConstParamN ns, long time) = 0;

    /**
      Registers a timer to be fired when event time watermark passes the given time. The namespace
      you pass here will be provided when the timer fires.
     */
    virtual void register_event_time_timer(ConstParamN ns, long time) = 0;

    /**
      Deletes the timer for the given key and namespace.
     */
    virtual void delete_event_time_timer(ConstParamN ns, long time) = 0;
};


