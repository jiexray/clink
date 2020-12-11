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
    // TODO: cancel a Task
    class OneTimeTask: boost::noncopyable {
    public:
        typedef std::function<void()> handler_fn;

        OneTimeTask(boost::asio::io_service& ioService, 
                std::string const& name, 
                long delay, 
                handler_fn task);

        ~OneTimeTask() {}

        void execute(boost::system::error_code const& e);

        /**
          Note: we have no guarantee that the timer will be cancelled, if the deadline is very close to the
            current time.
        
          @return: whether there is a handler to cancel.
         */
        bool cancel();
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
                handler_fn task);

        void execute(boost::system::error_code const& e);

        bool cancel();

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
        void worker_run();

        void run();

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
                int initial_delay = 0);

        /**
          Add one-time executing task.

          @param name The one-time task name.
          @param task The callback of execution.
          @param delay The delay of callback execution, ms.
         */
        std::unique_ptr<OneTimeTask> add_one_time_task(
                std::string const& name,
                PeriodicTask::handler_fn const& task,
                long delay);

        void stop();

        PeriodicScheduler();

        PeriodicScheduler(int num_of_thread);

    private:
        int m_num_of_thread;
        boost::thread_group m_worker_threads;
        boost::asio::io_service m_io_service;
        boost::asio::io_service::work* m_io_service_work;
        std::vector<std::unique_ptr<PeriodicTask>> tasks;
        std::vector<std::unique_ptr<OneTimeTask>> one_time_tasks;
    };

}