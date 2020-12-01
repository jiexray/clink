#pragma once

#include "cxxtest/TestSuite.h"
#include "Scheduler.hpp"
#include "TimeUtil.hpp"
#include "LoggerFactory.hpp"
#include <iostream>
#include <chrono>
#include <thread>

class TestScheduler: public CxxTest::TestSuite
{
public:
    void testLogger(void) {
        std::shared_ptr<spdlog::logger> logger = LoggerFactory::get_logger("TestLogger");
        // SPDLOG_LOGGER_INFO(logger, "Test logging");
    }

    void testOneTimeTask( void ) {
        std::cout << "test testOneTimeTask" << std::endl;
        Scheduler::PeriodicScheduler scheduler(2);
        scheduler.run();

        std::unique_ptr<Scheduler::OneTimeTask> timer_1 = scheduler.add_one_time_task("test-one-time-task", []() {
            std::cout << "[timer_1] Invoke test-one-time-task in thread " << std::this_thread::get_id() << ", at " << TimeUtil::current_timestamp() << std::endl;
        },
        200);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "on stopping scheduler" << std::endl;
        scheduler.stop();
    }

    void testOneTimeTaskCancel( void ) {
        std::cout << "test testOneTimeTaskCancel" << std::endl;
        Scheduler::PeriodicScheduler scheduler(2);
        scheduler.run();

        std::unique_ptr<Scheduler::OneTimeTask> timer_1 = scheduler.add_one_time_task("test-one-time-task", []() {
            std::cout << "[timer_1] Invoke test-one-time-task in thread " << std::this_thread::get_id() << ", at " << TimeUtil::current_timestamp() << std::endl;
        },
        200);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "on canceling timer_1" << std::endl;
        TS_ASSERT_EQUALS(timer_1->cancel(), true);

        timer_1 = scheduler.add_one_time_task("test-one-time-task", []() {
            std::cout << "[timer_2] Invoke test-one-time-task in thread " << std::this_thread::get_id() << ", at " << TimeUtil::current_timestamp() << std::endl;
        },
        100);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "on stopping scheduler" << std::endl;
        scheduler.stop();
    }

    void testPeriodicTask( void ) {
        std::cout << "test testPeriodicTask" << std::endl;
        Scheduler::PeriodicScheduler scheduler(2);
        scheduler.run();

        scheduler.add_periodic_task("test-period-task", []() {
            std::cout << "Period task running" << std::endl;
        }, 100, 100);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "on stopping scheduler" << std::endl;
        scheduler.stop();
    }
};

