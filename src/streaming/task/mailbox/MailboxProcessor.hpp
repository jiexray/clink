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
#include "MailboxDefaultAction.hpp"
#include <iostream>
#include <memory>


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

    volatile bool                           m_is_suspended = false;

    /**
      Calling this method signals that the mailbox-thread should stop invoking the default action.
     */
    void suspend_default_action() {
        if (!m_is_suspended) {
            m_is_suspended = true;

            // ensure_control_flow_signal_check();
        }
    }

    bool is_default_action_unavailable() {
        return m_is_suspended;
    }

    /**
       Helper method to make sure that the mailbox loop will check the control flow flags in the next iteration.
       However, I think it has no use.
     */
    void ensure_control_flow_signal_check() {
        if (!m_mailbox->has_mail()) {
            sendControlMail([]{}, "signal check");
        }
    }

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

    bool                                    run_mailbox_step(std::shared_ptr<MailboxDefaultAction::Controller> default_action_context);

    bool                                    process_mail();

    void                                    complete() {
        SPDLOG_LOGGER_DEBUG(m_logger, "complete mailbox");
        set_loop_running(false);
    }

    void                                    resume_internal() {
        SPDLOG_LOGGER_TRACE(m_logger, "resume default action");
        m_is_suspended = false;
    }

    /**
      Must call by InputGate consumer, direct set m_is_suspend
     */
    void                                    resume() {
        m_is_suspended = true;
        
        sendControlMail(std::bind(&MailboxProcessor::resume_internal, this), "resume default action");
    }

    /* This method must be called to end the stream task when all actions for the tasks have been performed */
    void                                    all_actions_completed();

    /* Send the given mail to maillist, intended use is to control this MailboxProcessor */
    void                                    sendControlMail(const Mail::MailFunc & runnable, std::string description) {
        SPDLOG_LOGGER_DEBUG(m_logger, "sent mail: {}", description);
        m_mailbox->put(std::make_shared<Mail>(runnable, description));
    }

    /* Properties */
    void                                    set_loop_running(bool val) {m_mailbox_loop_running = val;}



    class MailboxController : public MailboxDefaultAction::Controller {
    private:
        typedef std::shared_ptr<MailboxProcessor> MailboxProcessorPtr;
        MailboxProcessorPtr m_mailbox_processor;
    public:
        MailboxController(MailboxProcessorPtr mailbox_processor): m_mailbox_processor(mailbox_processor) {}

        void all_actions_completed() override {
            m_mailbox_processor->all_actions_completed();
        }
    
        void suspend_default_action() override{
            m_mailbox_processor->suspend_default_action();
        }
    };
};
