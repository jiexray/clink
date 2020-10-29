/**
 * Interface for the default action that is repeatedly invoked in the mailbox-loop.
 */
#pragma once
#include <memory>

class MailboxDefaultAction {
public:
    
    class Controller
    {
    public:
        virtual void all_actions_completed() = 0;
        // virtual std::shared_ptr<Suspension> suspend_default_action() = 0;
        virtual void suspend_default_action() = 0;
    }; 

    virtual void run_default_action(std::shared_ptr<Controller>) = 0;
};