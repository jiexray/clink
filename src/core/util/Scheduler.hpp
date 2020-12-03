/**
 * Scheduler utils for periodically tasks.
 */ 
#pragma once
#include <ctime>
#include <iostream>
#include <iomanip>
#include <memory>
#include <functional>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "TimeUtil.hpp"
#include "LoggerFactory.hpp"

namespace Scheduler {
    std::shared_ptr<spdlog::logger> logger = LoggerFactory::get_logger("Scheduler");
    // TODO: cancel a Task
    class OneTimeTask: boost::noncopyable {
    public:
        typedef std::function<void()> handler_fn;

        OneTimeTask(boost::asio::io_service& ioService, 
                std::string const& name, 
                long delay, 
                handler_fn task):
                ioService(ioService), 
                delay(delay), 
                task(task), 
                name(name), 
                timer(ioService) {
            SPDLOG_LOGGER_DEBUG(logger, "start OneTimeTask at {}", TimeUtil::current_timestamp());
            timer.expires_from_now(boost::posix_time::milliseconds(delay));
            timer.async_wait(boost::bind(
                &OneTimeTask::execute, 
                this, 
                boost::asio::placeholders::error));
        }

        ~OneTimeTask() {
        }

        void execute(boost::system::error_code const& e) {
            if (e != boost::asio::error::operation_aborted) {
                task();
            } else {
                SPDLOG_LOGGER_WARN(logger, "OneTimeTask execution error {}", e.message());
            }
        }

        /**
          Note: we have no guarantee that the timer will be cancelled, if the deadline is very close to the
            current time.
        
          @return: whether there is a handler to cancel.
         */
        bool cancel() {
            // SPDLOG_LOGGER_WARN(logger, "OneTimeTask cancel name {}", name);
            int handler_cnt = timer.cancel();
            return handler_cnt > 0;
        }
    private:
        boost::asio::io_service& ioService;
        boost::asio::deadline_timer timer;
        handler_fn task;
        std::string name;
        long delay;
    };

    class PeriodicTask : boost::noncopyable {
    public:
        typedef std::function<void()> handler_fn;

        PeriodicTask(boost::asio::io_service& ioService, 
                std::string const& name, 
                long interval, 
                long initial_delay,
                handler_fn task): 
                ioService(ioService), 
                interval(interval), 
                initial_delay(initial_delay),
                task(task), 
                name(name), 
                timer(ioService) {
            SPDLOG_LOGGER_INFO(logger, "start PeriodicTask {} at {}", name, TimeUtil::current_timestamp());
            timer.expires_from_now(boost::posix_time::milliseconds(initial_delay));
            timer.async_wait(boost::bind(
                    &PeriodicTask::execute, 
                    this, 
                    boost::asio::placeholders::error));
        }

        void execute(boost::system::error_code const& e) {
            if (e != boost::asio::error::operation_aborted) {
                task();
                timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(interval));
                timer.async_wait(boost::bind(
                        &PeriodicTask::execute, this, 
                        boost::asio::placeholders::error));
            } else {
                SPDLOG_LOGGER_DEBUG(logger, "PeriodicTask execution error {}", e.message());
            }
        }

        bool cancel() {
            int handler_cnt = timer.cancel();
            return handler_cnt > 0;
        }

    private:
        boost::asio::io_service& ioService;
        boost::asio::deadline_timer timer;
        handler_fn task;
        std::string name;
        long interval;
        long initial_delay;
    };

    class PeriodicScheduler : boost::noncopyable {
    public:
        void worker_run() {
            this->m_io_service.run();
        }

        void run() {
            SPDLOG_LOGGER_INFO(logger, "Start PeriodicScheduler with {} worker threads", m_num_of_thread);
            for (int i = 0; i < m_num_of_thread; i++) {
                // m_worker_threads.create_thread([this]() {
                //     SPDLOG_LOGGER_INFO(logger, "[{}] executor thread start", std::this_thread::get_id());
                //     this->m_io_service.run();
                //     SPDLOG_LOGGER_INFO(logger, "[{}] executor thread finish", std::this_thread::get_id());
                // });
                m_worker_threads.create_thread(std::bind(&PeriodicScheduler::worker_run, this));
            }
        }

        /**
          Add periodical executing task.

          @param name The periodic task name.
          @param task The callback of each execution.
          @param internal The interval between consecutive execution, ms.
          @param initial_delay The delay between initialization and first execution, ms.
         */
        void add_periodic_task(
            std::string const& name, 
                PeriodicTask::handler_fn const& task,
                int interval,
                int initial_delay = 0) {
            tasks.push_back(
                    std::make_unique<PeriodicTask>(
                    std::ref(m_io_service), 
                    name, 
                    interval, 
                    initial_delay,
                    task));
        }

        /**
          Add one-time executing task.

          @param name The one-time task name.
          @param task The callback of execution.
          @param delay The delay of callback execution, ms.
         */
        std::unique_ptr<OneTimeTask> add_one_time_task(
                std::string const& name,
                PeriodicTask::handler_fn const& task,
                long delay) {
            return std::make_unique<OneTimeTask>(
                    std::ref(m_io_service),
                    name, 
                    delay,
                    task);
        }

        void stop() {
            m_io_service.stop();
            delete m_io_service_work;
            m_worker_threads.join_all();
        }

        PeriodicScheduler(): PeriodicScheduler(1) {}

        PeriodicScheduler(int num_of_thread):
                m_num_of_thread(num_of_thread) {
            m_io_service_work = new boost::asio::io_service::work(m_io_service);
        }

    private:
        int m_num_of_thread;
        boost::thread_group m_worker_threads;
        boost::asio::io_service m_io_service;
        boost::asio::io_service::work* m_io_service_work;
        std::vector<std::unique_ptr<PeriodicTask>> tasks;
        std::vector<std::unique_ptr<OneTimeTask>> one_time_tasks;
    };

}