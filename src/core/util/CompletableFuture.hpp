/**
 * Utils for completable future.
 */
#pragma once
#include "LoggerFactory.hpp"
#include <future>
#include <memory>
#include <functional>
#include <vector>
#include <atomic>
#include <mutex>

template <typename T>
class CompletableFuture: public std::enable_shared_from_this<CompletableFuture<T>>
{
private:
    typedef std::function<void()> HandleFunc;
    typedef typename std::shared_ptr<CompletableFuture<T>> CompletableFuturePtr;
    typedef std::shared_ptr<CompletableFuture<bool>> CompletableFutureBoolPtr;

    std::future<T>   m_future_available;
    std::promise<T>  m_promise;
    HandleFunc       m_func;
    std::atomic_bool m_func_set;
    std::atomic_bool m_has_handle_run;
    std::atomic_bool m_is_done;
    std::mutex       m_lock;

    static std::shared_ptr<spdlog::logger> m_logger;


    bool is_ready(std::future<T> const& f){ 
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
    }

public:
    CompletableFuture() {
        m_promise = std::promise<T>();
        m_future_available = this->m_promise.get_future();

        m_has_handle_run.store(false);
        m_is_done.store(false);
        m_func_set.store(false);
        m_func = nullptr;
    }


    CompletableFuture(const T& val):
            CompletableFuture() {
        complete(val);
    }

    ~CompletableFuture() {
        // SPDLOG_LOGGER_DEBUG(m_logger, "destroy CF {:p}", (char*)(this));
        if (!m_is_done.load()) {
            std::cout << "destroy CF before it is available!" << std::endl;
            m_promise.set_value(true);
            m_is_done.store(true);
            SPDLOG_LOGGER_ERROR(m_logger, "destroy CF before it is available!");
            throw std::runtime_error("destroy CF before it is available!");
        }
    }

    bool is_done() {
        return m_is_done.load();
    }

    void complete(const T& value) {
        // std::unique_lock<std::mutex> lock(m_lock);

        bool has_done = m_is_done.exchange(true);
        if (has_done) {
            return;
        }
        this->m_promise.set_value(value);


        if (m_func_set.load()) {
            bool has_handled_cached = m_has_handle_run.exchange(true);

            if (!has_handled_cached) {
                // lock.unlock();
                m_func();
            }
        } 
    }

    // in parent thread
    void then(HandleFunc func) {
        // std::unique_lock<std::mutex> lock(m_lock);
        m_func = func;
        m_func_set.store(true);

        if (m_is_done.load()) {
            bool has_handled_cached = m_has_handle_run.exchange(true);

            if (!has_handled_cached) {
                // lock.unlock();
                m_func();
            }
        }
    }

    static CompletableFuturePtr complete_future(const T& val) {
        return std::make_shared<CompletableFuture<T>>(val);
    }
};

template<typename T>
std::shared_ptr<spdlog::logger> CompletableFuture<T>::m_logger = LoggerFactory::get_logger("CompletableFuture");