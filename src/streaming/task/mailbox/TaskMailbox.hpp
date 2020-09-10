/**
 * A task mailbox procides read and write access to a mailbox.
 */
#pragma once
#include <memory>
#include <deque>
#include <mutex>
#include <thread>
#include "Mail.hpp"

class TaskMailbox
{
private:
    std::deque<std::shared_ptr<Mail>>       m_queue;
    std::mutex                              m_queue_mutex;

    std::shared_ptr<std::thread>            m_task_mailbox_thread;

    // TODO: add a batch cache, which batch several mails into a batch, and do not need to lock queue 
    //       for each take. However, this need the take operation must conduct in task_mailbox_thread.

public:
    TaskMailbox(std::shared_ptr<std::thread> task_mailbox_thread): m_task_mailbox_thread(task_mailbox_thread) {}

    std::shared_ptr<Mail>                   try_take();

    void                                    put(std::shared_ptr<Mail> mail);
};
