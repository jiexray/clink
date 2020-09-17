/**
 * This class encapsulates the logic of the mailbox-based execution model. At the core of runMailboxLoop() that
 * containuously executes the provided MailboxDefaultAction in a loop. On each iteration, the method also checks 
 * if there are pending actions in the mailbox and executes such actions. This model ensures single-threaded
 * execution between the default action (e.g. record processing) and mailbox actions (e.g. checkpoint, trigger, 
 * timer firing, ...).
 */
#pragma once
#include "TaskMailbox.hpp"
#include "MailboxExecutor.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <iostream>
#include <memory>

class MailboxDefaultAction {
public:
    virtual void run_default_action() = 0;
};


class MailboxProcessor : public std::enable_shared_from_this<MailboxProcessor>
{
private:
    /* The mailbox data-structure that manangers request for special actions, like timers, checkpoints, ... */
    std::shared_ptr<TaskMailbox>            m_mailbox;

    /* Action that is repeatedly executed if no action request is in the mailbox. Typically record processing. */
    std::shared_ptr<MailboxDefaultAction>   m_mailbox_default_action;

    /* A pre-created instnace of mailbox executor that executes all mails */
    std::shared_ptr<MailboxExecutor>        m_mailbox_executor;
    
    bool                                    m_mailbox_loop_running;

    static std::shared_ptr<spdlog::logger>  m_logger;

    int                                     m_round;
public:
    MailboxProcessor(std::shared_ptr<MailboxDefaultAction> mailbox_default_action, std::shared_ptr<TaskMailbox> mailbox):
    m_mailbox(mailbox),
    m_mailbox_default_action(mailbox_default_action),
    m_mailbox_loop_running(true) {
        m_mailbox_executor = std::make_shared<MailboxExecutor>(mailbox);
        m_round = 0;
    }

    /* Runs the mailbox processing loop. This is where the main work is done. */
    void                                    run_mailbox_loop();

    bool                                    run_mailbox_step();

    bool                                    process_mail();

    /* This method must be called to end the stream task when all actions for the tasks have been performed */
    void                                    all_actions_completed();

    /* Send the given mail to maillist, intended use is to control this MailboxProcessor */
    void                                    sendControlMail(std::shared_ptr<Runnable> runnable, std::string descrption);

    /* Properties */
    void                                    set_loop_running(bool val) {m_mailbox_loop_running = val;}

    class AllCompleteMail : public Runnable {
    private: 
        std::shared_ptr<MailboxProcessor>   m_mailbox_processor;
    public:
        AllCompleteMail(std::shared_ptr<MailboxProcessor> mailbox_processor): m_mailbox_processor(mailbox_processor) {}

        void run() override {
            m_mailbox_processor->set_loop_running(false);
        }
    };
};
