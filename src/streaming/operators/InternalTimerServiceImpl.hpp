#pragma once

#include "InternalTimerService.hpp"
#include "ProcessingTimeService.hpp"
#include "KeyContext.hpp"
#include "TimerHeapInternalTimer.hpp"
#include "KeyGroupRange.hpp"
#include "Triggerable.hpp"
#include "Scheduler.hpp"
#include "LoggerFactory.hpp"
#include "HeapPriorityQueueSet.hpp"
#include <queue>
#include <memory>
#include <limits.h>

/**
  InternalTimerService that stores timers on the heap.
 */
template <class K, class N>
class InternalTimerServiceImpl: public InternalTimerService<N>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;

    ProcessingTimeService& m_processing_time_service;

    KeyContext<K>& m_key_context;

    HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>* m_processing_time_timers_queue;
    HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>* m_event_time_timers_queue;

    KeyGroupRange m_local_key_group_range;

    volatile bool m_is_initialized;

    Triggerable<K, N>& m_trigger_target;

    long m_current_watermark = LONG_MIN;

    // next_timer, which will be replaced if there is a more earlier timestamp to fire
    std::unique_ptr<Scheduler::OneTimeTask> m_next_timer = nullptr;

    static std::shared_ptr<spdlog::logger> m_logger;

public:
    InternalTimerServiceImpl(
            const KeyGroupRange& local_key_group_range, 
            KeyContext<K>& key_context, 
            ProcessingTimeService& processing_time_service,
            Triggerable<K, N>& trigger_target,
            HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>* processing_time_timers_queue,
            HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>* event_time_timers_queue):
            m_processing_time_service(processing_time_service),
            m_local_key_group_range(local_key_group_range),
            m_key_context(key_context),
            m_trigger_target(trigger_target),
            m_processing_time_timers_queue(processing_time_timers_queue),
            m_event_time_timers_queue(event_time_timers_queue) {
        
    }
    
    ~InternalTimerServiceImpl() {
        if (m_next_timer != nullptr){
            // TODO: release next_timer
            // m_next_timer->cancel();
            m_next_timer.release();
            // m_next_timer.reset();
        }
        
        delete m_processing_time_timers_queue;
        delete m_event_time_timers_queue;
    }

    void start_timer_service() {
        // TODO: implement timers restore
    }

    long current_processing_time() override {
        return m_processing_time_service.get_current_processing_time();
    }

    long current_watermark() override {
        return m_current_watermark;
    }

    /**
      Maybe a bug:
      (1) WindowOperator thread is preempted, and the timer is not inserted into timers' priority_queue
      (2) ProcessingTimeService thread (Executor in Scheduler) starts to run, and finishes executing one round of on_processing_time
           while the new timer at (1) is not noticed.
      (1) WindowOperator thread back to work, find the next trigger time is smaller than new time, skip the if statement at (3)

      Flink design to resolve the bug:
      Check whether the new item is the head in the priority queue after inserting new timer into m_processing_time_timer_queue
     */


    void register_processing_time_timer(ConstParamN ns, long time) override {
        // check the integrity of next_timer and timer_queues
        if (m_next_timer == nullptr && !m_processing_time_timers_queue->empty()) {
            // SPDLOG_LOGGER_WARN(m_logger, "next_timer is not set (nullptr), while timer queue is not empty (top timer {}), register time {}", m_processing_time_timers_queue->top().get_timestamp(), time);
        }

        long next_trigger_time = m_processing_time_timers_queue->empty() ? 
                                    LONG_MAX:
                                    m_processing_time_timers_queue->top().get_timestamp();

        m_processing_time_timers_queue->add(TimerHeapInternalTimer<K, N>(time, m_key_context.get_current_key(), ns));
        // m_processing_time_timers_queue->emplace(time, m_key_context.get_current_key(), ns);  // (1)

        long next_trigger_time_after_insertion = m_processing_time_timers_queue->empty() ? 
                                    LONG_MAX:
                                    m_processing_time_timers_queue->top().get_timestamp();

        if (next_trigger_time != LONG_MAX) {
            assert(next_trigger_time == next_trigger_time_after_insertion);
        }

        if (time < next_trigger_time) { // (3)
            // register a earlier timer to notify the trigger processing-time arrived
            if (m_next_timer != nullptr) {
                SPDLOG_LOGGER_DEBUG(m_logger, "get a new timer earlier {} than previous timer {}, try to cancel next_timer", time, next_trigger_time);
                // cancel the later timer, not thread-safe, the timer cancellation may fail
                m_next_timer->cancel();
            }
            m_next_timer.reset();
            m_next_timer = this->m_processing_time_service.register_timer(time, std::bind(&InternalTimerServiceImpl<K, N>::on_processing_time, this, std::placeholders::_1));
        }
    }

    void delete_processing_time_timer(ConstParamN ns, long time) override {
        // TODO: delete element from m_processing_time_timers_queue
    }

    void register_event_time_timer(ConstParamN ns, long time) override {
        // this->m_event_time_timers_queue->emplace(time, m_key_context.get_current_key(), ns);
        this->m_event_time_timers_queue->add(TimerHeapInternalTimer<K, N>(time, m_key_context.get_current_key(), ns));
    }

    void delete_event_time_timer(ConstParamN ns, long time) override {
        // TODO: delete element from m_event_time_timers_queue
    }

    void on_processing_time(long time) {
        // null out the timer in case the Triggerable calls register_processing_time_timer()
        // inside the callback.
        SPDLOG_LOGGER_DEBUG(m_logger, "on_processing_time() trigger timer {}", time);
        this->m_next_timer.reset(nullptr);

        while(!this->m_processing_time_timers_queue->empty()) { // (2)
            const InternalTimer<K, N>& timer = this->m_processing_time_timers_queue->top();
            if (timer.get_timestamp() <= time) {
                this->m_key_context.set_current_key(timer.get_key());
                /**
                 Note: the current thread will invoke the Trigger one by one in a sequenical way.
                Therefore, even though we register several triggering timestamp, the trigger events will
                happens in sequence. In summary, we do not need to synchronize the excution of Triggers.
                They will be invoked in time order.
                */
                this->m_trigger_target.on_processing_time(timer);
                this->m_processing_time_timers_queue->pop();
            } else {
                if (m_next_timer == nullptr) {
                    SPDLOG_LOGGER_DEBUG(m_logger, "on_processing_time() trigger timer {}, and re-register a timer {} to timer priority_queue", time, timer.get_timestamp());
                    m_next_timer = this->m_processing_time_service.register_timer(timer.get_timestamp(), std::bind(&InternalTimerServiceImpl<K, N>::on_processing_time, this, std::placeholders::_1));
                }
                break;
            }
        }
    }

    void advance_watermark(long time) {
        m_current_watermark = time;

        while(!this->m_event_time_timers_queue->empty()) { // (2)
            const InternalTimer<K, N>& timer = this->m_event_time_timers_queue->top();
            if (timer.get_timestamp() < time) {
                this->m_key_context.set_current_key(timer.get_key());
                this->m_trigger_target.on_event_time(timer);
                this->m_event_time_timers_queue->pop();
            } else {
                break;
            }
        }
    }
};

template <class K, class N>
std::shared_ptr<spdlog::logger> InternalTimerServiceImpl<K, N>::m_logger = LoggerFactory::get_logger("InternalTimerServiceImpl");

