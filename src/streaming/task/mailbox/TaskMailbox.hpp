/**
 * A task mailbox procides read and write access to a mailbox.
 */
#pragma once
#include <memory>
#include <deque>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>
#include "Mail.hpp"
#include "LoggerFactory.hpp"

class TaskMailbox
{
private:
    std::deque<std::shared_ptr<Mail>>       m_queue;
    std::mutex                              m_queue_mutex;

    std::shared_ptr<std::thread>            m_task_mailbox_thread;
    std::condition_variable                 m_available_condition_variable;

    volatile bool                           has_new_mail;

    static std::shared_ptr<spdlog::logger>  m_logger;

    // TODO: add a batch cache, which batch several mails into a batch, and do not need to lock queue 
    //       for each take. However, this need the take operation must conduct in task_mailbox_thread.

public:
    TaskMailbox(std::shared_ptr<std::thread> task_mailbox_thread): m_task_mailbox_thread(task_mailbox_thread) {has_new_mail = false;}

    std::shared_ptr<Mail>                   try_take();

    std::shared_ptr<Mail>                   take();

    void                                    put(std::shared_ptr<Mail> mail);

    bool                                    has_mail() {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        return has_new_mail;
    }
};
