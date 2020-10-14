/**
 * The ViewUpdater is responsible for updating all metrics that implement the View interface.
 */
#pragma once
#include "View.hpp"
#include <set>
#include <memory>
#include <mutex>

class ViewUpdater
{
private:
    std::set<std::shared_ptr<View>>     m_to_add;
    std::set<std::shared_ptr<View>>     m_to_remove;
    std::set<std::shared_ptr<View>>     m_views;

    std::mutex                          m_lock;
public:
    ViewUpdater() {
        // TODO: start the ViewUpdater with Timer
    }

    void                                notify_of_add_view(std::shared_ptr<View> view) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_to_add.insert(view);
    }

    void                                notify_of_remove_view(std::shared_ptr<View> view) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_to_remove.insert(view);
    }

    // TODO: schedule #run in a Timer
    void run() {
        for(std::shared_ptr<View> view: m_views) {
            view->update();
        }

        std::unique_lock<std::mutex> lock(m_lock);
        m_views.insert(m_to_add.begin(), m_to_add.end());
        m_to_add.clear();
        m_views.erase(m_to_remove.begin(), m_to_remove.end());
        m_to_remove.clear();
    }
};
