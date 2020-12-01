#pragma once
#include "KeyGroupRange.hpp"
#include "KeyContext.hpp"
#include "ProcessingTimeService.hpp"
#include "InternalTimerServiceImpl.hpp"
#include <map>

/**
  An entity keeping all the timer-related services available to all operaots extending the 
  AbstractStreamOperator. Right now, this is only a InternalTimerServiceImpl timer service.
 */
template <class K, class N>
class InternalTimeServiceManager {
private:
    KeyGroupRange m_local_key_group_range;
    KeyContext<K>& m_key_context;

    ProcessingTimeService& m_processing_time_service;

    std::map<std::string, InternalTimerServiceImpl<K, N>*> m_timer_services;

public:
    InternalTimeServiceManager(
            const KeyGroupRange& key_group_range, 
            KeyContext<K>& key_context, 
            ProcessingTimeService& processing_time_service):
            m_local_key_group_range(key_group_range),
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
                    triggerable);
            m_timer_services[name] = timer_service;
            return timer_service;
        }
    }
};
