#pragma once

#include "cxxtest/TestSuite.h"
#include "AvailabilityProvider.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <ctime>
#include <cstdlib>
#include <future>

class FakeAvailablilityClass: public AvailabilityProvider {
    AvailabilityProvider::CompletableFuturePtr get_available_future() {
        
    }
};

void trigger_available(std::shared_ptr<CompletableFuture<bool>> complete_future, const int* seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(*seconds));
    complete_future->complete(true);
}

void notice_available() {
    std::cout << "is available" << std::endl;
}

class TestAvailable: public CxxTest::TestSuite
{
    typedef AvailabilityProvider::AvailabilityHelper AvailabilityHelper;
    typedef std::shared_ptr<AvailabilityProvider> AvailabilityProviderPtr;
    typedef typename std::shared_ptr<CompletableFuture<bool>> CompletableFuturePtr;
public:
    void testFakeAvailable () {
        FakeAvailablilityClass availability;
        CompletableFuturePtr f = availability.AVAILABLE;
        TS_ASSERT_EQUALS(f->is_done(), true);
    }

    void testCompleteFutureThen( void ) {
        std::cout << "test testCompleteFutureThen()" << std::endl;

        CompletableFuturePtr complete_future = std::make_shared<CompletableFuture<bool>>();

        std::thread t = std::thread(std::bind(&trigger_available, complete_future, new int(2)));

        std::cout << "attach function to CompleteFuture" << std::endl;

        complete_future->then(notice_available);

        std::cout << "start waiting for complete" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "finish waiting for complete" << std::endl;

        t.join();
    }
};

