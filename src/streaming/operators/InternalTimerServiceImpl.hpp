#pragma once

#include "InternalTimerService.hpp"
#include "ProcessingTimeService.hpp"
#include "KeyContext.hpp"
#include "TimerHeapInternalTimer.hpp"
#include "KeyGroupRange.hpp"
#include "Triggerable.hpp"
#include "Scheduler.hpp"
#include <queue>
#include <memory>
#include <limits.h>

void test_func(long time) {}

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

    std::priority_queue<TimerHeapInternalTimer<K, N>> m_processing_time_timers_queue;
    std::priority_queue<TimerHeapInternalTimer<K, N>> m_event_time_timers_queue;

    KeyGroupRange m_local_key_group_range;

    volatile bool m_is_initialized;

    Triggerable<K, N>& m_trigger_target;

    long m_current_watermark = LONG_MIN;

    // next_timer, which will be replaced if there is a more earlier timestamp to fire
    std::unique_ptr<Scheduler::OneTimeTask> m_next_timer = nullptr;

public:
    InternalTimerServiceImpl(
            const KeyGroupRange& local_key_group_range, 
            KeyContext<K>& key_context, 
            ProcessingTimeService& processing_time_service,
            Triggerable<K, N>& trigger_target):
            m_processing_time_service(processing_time_service),
            m_local_key_group_range(local_key_group_range),
            m_key_context(key_context),
            m_trigger_target(trigger_target) {
        
    }
    
    ~InternalTimerServiceImpl() {
        if (m_next_timer != nullptr){
            // TODO: release next_timer
            // m_next_timer->cancel();
            m_next_timer.release();
            // m_next_timer.reset();
        }
        
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

    void on_processing_time(long time) {
        std::cout << "InternalTimerServiceImpl on_processing_time()" << std::endl;
        while(!this->m_processing_time_timers_queue.empty()) {
            const InternalTimer<K, N>& timer = this->m_processing_time_timers_queue.top();
            if (timer.get_timestamp() <= time) {
                this->m_key_context.set_current_key(timer.get_key());
                /**
                 Note: the current thread will invoke the Trigger one by one in a sequenical way.
                Therefore, even though we register several triggering timestamp, the trigger events will
                happens in sequence. In summary, we do not need to synchronize the excution of Triggers.
                They will be invoked in time order.
                */
                this->m_trigger_target.on_processing_time(timer);
                this->m_processing_time_timers_queue.pop();
            } else {
                break;
            }
        }
    }

    void register_processing_time_timer(ConstParamN ns, long time) override {
        long next_trigger_time = m_processing_time_timers_queue.empty() ? 
                                    LONG_MAX:
                                    m_processing_time_timers_queue.top().get_timestamp();

        // m_processing_time_timers_queue.push(TimerHeapInternalTimer<K, N>(time, m_key_context.get_current_key(), ns));
        m_processing_time_timers_queue.emplace(time, m_key_context.get_current_key(), ns);

        if (time < next_trigger_time) {
            // register a earlier timer to notify the trigger processing-time arrived
            if (m_next_timer != nullptr) {
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
        this->m_event_time_timers_queue.push(TimerHeapInternalTimer<K, N>(time, m_key_context.get_current_key(), ns));
    }

    void delete_event_time_timer(ConstParamN ns, long time) override {
        // TODO: delete element from m_event_time_timers_queue
    }
};

