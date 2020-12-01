#pragma once

#include "ProcessingTimeService.hpp"
#include <atomic>
#include "Scheduler.hpp"
#include "TimeUtil.hpp"
#include <algorithm>
#include <iostream>
#include <thread>

class SystemProcessingTimeService: public ProcessingTimeService
{
private:
    typedef Scheduler::PeriodicScheduler            PeriodicScheduler;
    static const int STATUS_ALIVE = 0;
    static const int STATUS_SHUTDOWN = 2;

    Scheduler::PeriodicScheduler* m_timer_service;

    std::atomic_int m_status;
    // std::thread m_timer_service_executor;

public:
    SystemProcessingTimeService() {
        m_status.store(STATUS_ALIVE);

        m_timer_service = new Scheduler::PeriodicScheduler();
        m_timer_service->run();
        // m_timer_service_executor = std::thread(std::bind(&PeriodicScheduler::run, m_timer_service));
    }

    ~SystemProcessingTimeService() {
        m_timer_service->stop();
        // m_timer_service_executor.join();
        delete m_timer_service;
    }

    long get_current_processing_time() override {
        return TimeUtil::current_timestamp();
    }

    std::unique_ptr<Scheduler::OneTimeTask> register_timer(long timestamp, ProcessingTimeCallback callback) override {
        long delay = std::max(timestamp - get_current_processing_time(), 0l) + 1;

        std::cout << "Register an one-time task, delay: " << delay << std::endl;
        return m_timer_service->add_one_time_task("processing-timer-one-time", std::bind(callback, timestamp), delay);
    }

    void schedule_at_fixed_rate(ProcessingTimeCallback callback, long initial_delay, long period) override {
        long next_timestamp = get_current_processing_time() + initial_delay;

        m_timer_service->add_periodic_task("processing-timer-fix-rate", std::bind(callback, next_timestamp), period, initial_delay);
    }

    bool is_alive() {
        return m_status.load() == STATUS_ALIVE;
    }

    bool is_terminated() override {
        return m_status.load() == STATUS_SHUTDOWN;
    }

    void shutdown_service() override {
        m_timer_service->stop();
        delete m_timer_service;
    }
};
