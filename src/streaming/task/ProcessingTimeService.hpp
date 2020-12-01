#pragma once
#include <functional>
#include "Scheduler.hpp"

/**
  Defines the current processing time and handles all related actions,
  such as register timers for tasks to be executed in the future.
 */
class ProcessingTimeService {
public:
    typedef std::function<void(long)> ProcessingTimeCallback;

    virtual long get_current_processing_time() = 0;

    virtual std::unique_ptr<Scheduler::OneTimeTask> register_timer(long timestamp, ProcessingTimeCallback target) = 0;

    virtual void schedule_at_fixed_rate(ProcessingTimeCallback target, long initial_delay, long period) = 0;

    virtual bool is_terminated() = 0;
    
    virtual void shutdown_service() = 0;

    virtual ~ProcessingTimeService() = default;
};

