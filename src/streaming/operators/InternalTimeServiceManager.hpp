#pragma once
#include "KeyGroupRange.hpp"
#include "KeyContext.hpp"
#include "ProcessingTimeService.hpp"
#include "InternalTimerServiceImpl.hpp"
#include "HeapPriorityQueueSet.hpp"
#include "TimerHeapInternalTimer.hpp"
#include <map>

/**
  An entity keeping all the timer-related services available to all operaots extending the 
  AbstractStreamOperator. Right now, this is only a InternalTimerServiceImpl timer service.
 */
template <class K, class N>
class InternalTimeServiceManager {
private:
    KeyGroupRange m_local_key_group_range;

    int m_total_number_of_key_groups;

    KeyContext<K>& m_key_context;

    ProcessingTimeService& m_processing_time_service;

    std::map<std::string, InternalTimerServiceImpl<K, N>*> m_timer_services;

public:
    InternalTimeServiceManager(
            const KeyGroupRange& key_group_range, 
            int total_number_of_key_groups,
            KeyContext<K>& key_context, 
            ProcessingTimeService& processing_time_service):
            m_local_key_group_range(key_group_range),
            m_total_number_of_key_groups(total_number_of_key_groups),
            m_key_context(key_context),
            m_processing_time_service(processing_time_service) {
    }

    ~InternalTimeServiceManager() {
        for (auto iter: m_timer_services) {
            delete iter.second;
        }
    }

    InternalTimerService<N>& get_internal_timer_service(
            const std::string& name, 
            Triggerable<K, N>& triggerable) {
        InternalTimerServiceImpl<K, N>* timer_service = register_or_get_timer_service(name, triggerable);

        timer_service->start_timer_service();

        return *timer_service;
    }

    InternalTimerServiceImpl<K, N>* register_or_get_timer_service(const std::string& name, Triggerable<K, N>& triggerable) {
        if (m_timer_services.find(name) != m_timer_services.end()) {
            return m_timer_services[name];
        } else {
            InternalTimerServiceImpl<K, N>* timer_service = new InternalTimerServiceImpl<K, N>(
                    m_local_key_group_range, 
                    m_key_context, 
                    m_processing_time_service, 
                    triggerable,
                    create_timer_priority_queue(),
                    create_timer_priority_queue()
                    );
            m_timer_services[name] = timer_service;
            return timer_service;
        }
    }

    void advance_watermark(long timestamp) {
        typename std::map<std::string, InternalTimerServiceImpl<K, N>*>::iterator it = m_timer_services.begin();
        while(it != m_timer_services.end()) {
            it->second->advance_watermark(timestamp);
            ++it;
        }
    }

    HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>* create_timer_priority_queue() {
        return new HeapPriorityQueueSet<TimerHeapInternalTimer<K, N>>(
            128,// minimum_capacity
            m_local_key_group_range,// key_group_range
            m_total_number_of_key_groups// total_number_of_groups
        );
    }
};
