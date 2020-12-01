#pragma once
#include "InternalTimer.hpp"
#include "TimeUtil.hpp"
#include <iostream>
#include <thread>

/**
  Interface for things that can be called by InternalTimerService.

  @param <K> Type of the keys to which timers are scoped.
  @param <N> Type of the namespace to which timers are scoped.
 */
template <class K, class N>
class Triggerable
{
public:
    virtual void on_event_time(const InternalTimer<K, N>& timer) = 0;

    virtual void on_processing_time(const InternalTimer<K, N>& timer) = 0;
};


template <class K, class N>
class TestTriggerable: public Triggerable<K, N> {
public:
    void on_event_time(const InternalTimer<K, N>& timer) override {
        std::cout << "TestTriggerable::on_event_time()" << std::endl;
    }

    void on_processing_time(const InternalTimer<K, N>& timer) override {
        std::cout << "TestTriggerable::on_processing_time(), current timestamp: " << TimeUtil::current_timestamp() << ", thread:" << std::this_thread::get_id() << std::endl;
    }
};