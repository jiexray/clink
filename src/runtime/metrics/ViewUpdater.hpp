/**
 * The ViewUpdater is responsible for updating all metrics that implement the View interface.
 */
#pragma once
#include "View.hpp"
#include <set>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

class ViewUpdater
{
private:
    typedef std::shared_ptr<boost::asio::deadline_timer> TimerPtr;

    std::set<std::shared_ptr<View>>     m_to_add;
    std::set<std::shared_ptr<View>>     m_to_remove;
    std::set<std::shared_ptr<View>>     m_views;

    std::mutex                          m_lock;

    TimerPtr                            m_timer;
public:
    ViewUpdater(boost::asio::io_service& io_service) {
        // start the ViewUpdater with Timer
        m_timer = std::make_shared<boost::asio::deadline_timer>(io_service, View::UPDATE_INTERVAL_SECONDS_INTERVAL);

        m_timer->async_wait(std::bind(&ViewUpdater::run, this));
    }

    void                                start(boost::asio::io_service& io_service) {
        boost::posix_time::seconds  interval(View::UPDATE_INTERVAL_SECONDS);
        boost::asio::deadline_timer timer(io_service, interval);

        timer.async_wait(std::bind(&ViewUpdater::run, this));
    }

    void                                notify_of_add_view(std::shared_ptr<View> view) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_to_add.insert(view);
    }

    void                                notify_of_remove_view(std::shared_ptr<View> view) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_to_remove.insert(view);
    }

    // schedule #run in a Timer
    void run() {
        std::cout << "UpdateView::run()" << std::endl;
        for(std::shared_ptr<View> view: m_views) {
            view->update();
        }

        std::unique_lock<std::mutex> lock(m_lock);
        m_views.insert(m_to_add.begin(), m_to_add.end());
        m_to_add.clear();
        m_views.erase(m_to_remove.begin(), m_to_remove.end());
        m_to_remove.clear();

        m_timer->expires_at(m_timer->expires_at() + View::UPDATE_INTERVAL_SECONDS_INTERVAL);
        m_timer->async_wait(boost::bind(&ViewUpdater::run, this));
    }
};
