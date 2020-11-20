#pragma once
#include "TriggerResult.hpp"
#include <string>

/**
  A context object that is given to Trigger method to allow them to register 
  timer callbacks and deal with state.
 */
class TriggerContext
{
public:
    virtual long get_current_processing_time() = 0;

    /**
      Returns the current watermark time.
     */
    virtual long get_current_watermark() = 0;

    /**
      Register a system time callback. When the current system time passes the specified
      time Trigger::on_process_time(long, Window, TriggerContext) is called with the time
      specified here.
     */
    virtual void register_processing_time_timer(long time) = 0;


    /**
      Register a event time callback. When the current watermark passes the specified
      time Trigger::on_event_time(long, Window, TriggerContext) is called with the time
      specified here.
     */
    virtual void register_event_time_timer(long time) = 0;

    /**
      Delete the processing time trigger for the given time.
     */
    virtual void delete_processing_time_timer(long time) = 0;

    /**
     Delete the event-time trigger for the given time.
     */
    virtual void delete_event_time_timer(long time) = 0;
};


/**
  A Trigger determines when a pane of a window should be evaluated to emit the 
  result for that part of window.

  @param <T> The type of elements on which this Trigger works.
  @param <W> The type of Windows on which this Trigger can operate.
 */
template <class T, class W>
class Trigger
{
public:
    /**
      Called for every element that gets added to a pane. The result of this will determine
      whether the pane is evaluated to emit results.

      @param element The element that arrived.
      @param timestamp The timestamp of the element that arrived.
      @param window The window to which the element is being added.
      @param ctx A context object that can be used to register time callbacks.
     */
    virtual TriggerResult on_element(T* element, long timestamp, W& window, TriggerContext& ctx) = 0;

    /**
      Called when a processing-time timer that was set using the trigger context fires.

      @param time The timestamp at which the timer fired.
      @param window The window for which the timer fired.
      @param ctx A context object that can be used to register timer callbacks.
     */
    virtual TriggerResult on_processing_time(long time, W& window, TriggerContext& ctx) = 0;

    /**
      Called when an event-time timer that was set using the trigger context fires.

      @param time The timestamp at which the timer fired.
      @param window The window for which the timer fire.
      @param ctx A context object that can be used to register timer callbacks.
     */
    virtual TriggerResult on_event_time(long time, W& window, TriggerResult& ctx) = 0;

    /**
      Clears any state that the trigger might still hold for the given window.
     */
    virtual void clear(W& window, TriggerContext& ctx) = 0;

    virtual std::string to_string() = 0;
};

