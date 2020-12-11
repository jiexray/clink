#include "Scheduler.hpp"
 
std::shared_ptr<spdlog::logger> logger = LoggerFactory::get_logger("Scheduler");

Scheduler::OneTimeTask::OneTimeTask(boost::asio::io_service& ioService, 
        std::string const& name, 
        long delay, 
        handler_fn task):
        ioService(ioService), 
        delay(delay), 
        task(task), 
        name(name), 
        timer(ioService) {
    SPDLOG_LOGGER_TRACE(logger, "start OneTimeTask {} at {}, delay {}", name, TimeUtil::current_timestamp(), delay);
    timer.expires_from_now(boost::posix_time::milliseconds(delay));
    timer.async_wait(boost::bind(
        &OneTimeTask::execute, 
        this, 
        boost::asio::placeholders::error));
}

void Scheduler::OneTimeTask::execute(boost::system::error_code const& e) {
    if (e != boost::asio::error::operation_aborted) {
        task();
    } else {
        SPDLOG_LOGGER_WARN(logger, "OneTimeTask execution error {}", e.message());
    }
}

bool Scheduler::OneTimeTask::cancel() {
    SPDLOG_LOGGER_WARN(logger, "OneTimeTask cancel name {}", name);
    int handler_cnt = timer.cancel();
    return handler_cnt > 0;
}

Scheduler::PeriodicTask::PeriodicTask(boost::asio::io_service& ioService, 
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

void Scheduler::PeriodicTask::execute(boost::system::error_code const& e) {
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

bool Scheduler::PeriodicTask::cancel() {
    int handler_cnt = timer.cancel();
    return handler_cnt > 0;
}

void Scheduler::PeriodicScheduler::worker_run() {
    this->m_io_service.run();
}

void Scheduler::PeriodicScheduler::run() {
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

void Scheduler::PeriodicScheduler::add_periodic_task(
    std::string const& name, 
        PeriodicTask::handler_fn const& task,
        int interval,
        int initial_delay) {
    tasks.push_back(
            std::make_unique<PeriodicTask>(
            std::ref(m_io_service), 
            name, 
            interval, 
            initial_delay,
            task));
}

std::unique_ptr<Scheduler::OneTimeTask> Scheduler::PeriodicScheduler::add_one_time_task(
        std::string const& name,
        PeriodicTask::handler_fn const& task,
        long delay) {
    return std::make_unique<OneTimeTask>(
            std::ref(m_io_service),
            name, 
            delay,
            task);
}

void Scheduler::PeriodicScheduler::stop() {
    m_io_service.stop();
    delete m_io_service_work;
    m_worker_threads.join_all();
}

Scheduler::PeriodicScheduler::PeriodicScheduler(): PeriodicScheduler(1) {}

Scheduler::PeriodicScheduler::PeriodicScheduler(int num_of_thread):
        m_num_of_thread(num_of_thread) {
    m_io_service_work = new boost::asio::io_service::work(m_io_service);
}