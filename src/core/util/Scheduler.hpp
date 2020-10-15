/**
 * Scheduler utils for periodically tasks.
 */ 
#pragma once
#include <ctime>
#include <iostream>
#include <iomanip>
#include <memory>
#include <functional>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

namespace Scheduler {
    class PeriodicTask : boost::noncopyable
    {
    public:
        typedef std::function<void()> handler_fn;

        PeriodicTask(boost::asio::io_service& ioService
            , std::string const& name
            , int interval
            , handler_fn task)
            : ioService(ioService)
            , interval(interval)
            , task(task)
            , name(name)
            , timer(ioService) {
           std:: cout << "Create PeriodicTask '" << name << "'" << std::endl;
            // Schedule start to be ran by the io_service
            ioService.post(boost::bind(&PeriodicTask::start, this));
        }

        void execute(boost::system::error_code const& e) {
            if (e != boost::asio::error::operation_aborted) {
                // std::cout << "Execute PeriodicTask '" << name << "'" << std::endl;

                task();

                timer.expires_at(timer.expires_at() + boost::posix_time::seconds(interval));
                start_wait();
            }
        }

        void start() {
            std::cout << "Start PeriodicTask '" << name << "'" << std::endl;

            // Uncomment if you want to call the handler on startup (i.e. at time 0)
            // task();

            timer.expires_from_now(boost::posix_time::seconds(interval));
            start_wait();
        }

        private:
            void start_wait() {
                timer.async_wait(boost::bind(&PeriodicTask::execute
                    , this
                    , boost::asio::placeholders::error));
            }

        private:
            boost::asio::io_service& ioService;
            boost::asio::deadline_timer timer;
            handler_fn task;
            std::string name;
            int interval;
    };

    class PeriodicScheduler : boost::noncopyable {
    public:
        void run() {
            m_io_service.run();
        }

        void add_task(std::string const& name
            , PeriodicTask::handler_fn const& task
            , int interval) {
            tasks.push_back(std::make_unique<PeriodicTask>(std::ref(m_io_service)
                , name, interval, task));
        }

        void stop() {
            m_io_service.stop();
            delete m_io_service_work;
        }

        PeriodicScheduler() {
            m_io_service_work = new boost::asio::io_service::work(m_io_service);
        }

    private:
        boost::asio::io_service m_io_service;
        boost::asio::io_service::work* m_io_service_work;
        std::vector<std::unique_ptr<PeriodicTask>> tasks;
    };

}