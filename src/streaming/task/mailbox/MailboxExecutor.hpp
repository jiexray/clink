/**
 * 
 */
#pragma once

#include "TaskMailbox.hpp"


class MailboxExecutor
{
private:
    std::shared_ptr<TaskMailbox>        m_task_mailbox;

public:
    MailboxExecutor(std::shared_ptr<TaskMailbox> mailbox):
    m_task_mailbox(mailbox) {}

    void execute(const Mail::MailFunc & command, std::string description) {
        m_task_mailbox->put(std::make_shared<Mail>(command, description));
    }

    bool try_yield() {
        std::shared_ptr<Mail> mail = m_task_mailbox->try_take();

        if (mail != nullptr) {
            mail->run();
            return true;
        } else {
            return false;
        }
    }
};


