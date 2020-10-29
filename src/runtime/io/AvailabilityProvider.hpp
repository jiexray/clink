/**
 * Interface defining couple of essential methods for listening on data availability using Future.
 */
#pragma once
#include <future>
#include <memory>
#include <iostream>
#include "CompletableFuture.hpp"
#include <mutex>

class AvailabilityProvider
{
public:
    typedef typename std::shared_ptr<CompletableFuture<bool>> CompletableFuturePtr;

    static CompletableFuturePtr AVAILABLE;

    virtual CompletableFuturePtr get_available_future() = 0;
    

    virtual bool is_available() {
        CompletableFuturePtr f = get_available_future();
        return f->is_done();
    }

    class AvailabilityHelper
    {
    private:
        typedef typename std::shared_ptr<CompletableFuture<bool>> CompletableFuturePtr;
        typedef std::shared_ptr<AvailabilityProvider> AvailableProviderPtr;
        CompletableFuturePtr                    m_available_future;

        std::shared_ptr<AvailabilityProvider>   m_provider;
        std::mutex                              m_lock;
    public:
        AvailabilityHelper(AvailableProviderPtr provider): m_provider(provider) {
            m_available_future = std::make_shared<CompletableFuture<bool>>();
        }
    
        void reset_unavailable() {
            std::unique_lock<std::mutex> lock(m_lock);
            if (m_available_future->is_done()) {
                m_available_future = std::make_shared<CompletableFuture<bool>>();
            }
        }

        CompletableFuturePtr get_unavailable_to_reset_available() {
            std::unique_lock<std::mutex> lock(m_lock);
            CompletableFuturePtr to_notify = m_available_future;

            // TODO: AVAILABLE's has_run is wrong    
            m_available_future = CompletableFuture<bool>::complete_future(true);

            return to_notify;
        }

        CompletableFuturePtr get_available_future() {
            std::unique_lock<std::mutex> lock(m_lock);
            return m_available_future;
        }
    };
    

};
