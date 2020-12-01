#pragma once

#include "SystemProcessingTimeService.hpp"
#include "ProcessingTimeService.hpp"
#include <atomic>
#include <iostream>


class ProcessingTimeServiceImpl: public ProcessingTimeService {
private:
    SystemProcessingTimeService& m_timer_service;

    std::atomic_int m_num_running_timers;

public:
    ProcessingTimeServiceImpl(SystemProcessingTimeService& timer_service):
            m_timer_service(timer_service) {
        m_num_running_timers.store(0);
    }

    long get_current_processing_time() override {
        return m_timer_service.get_current_processing_time();
    }

    std::unique_ptr<Scheduler::OneTimeTask> register_timer(long timestamp, ProcessingTimeCallback target) override {
        return m_timer_service.register_timer(timestamp, target);
    }

    void schedule_at_fixed_rate(ProcessingTimeCallback target, long initial_delay, long period) override {
        m_timer_service.schedule_at_fixed_rate(target, initial_delay, period);
    }

    bool is_terminated() override {
        return m_timer_service.is_terminated();
    }

    void shutdown_service() override {
        m_timer_service.shutdown_service();
    }
};