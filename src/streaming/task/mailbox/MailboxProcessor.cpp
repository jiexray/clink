#include "MailboxProcessor.hpp"

void MailboxProcessor::run_mailbox_loop() {
    while(run_mailbox_step()){}
}

bool MailboxProcessor::run_mailbox_step() {
    if (process_mail()) {
        m_mailbox_default_action->run_default_action();
        return true;
    } else {
        return false;
    }
}

bool MailboxProcessor::process_mail() {
    std::shared_ptr<Mail> maybe_mail = m_mailbox->try_take();

    if (maybe_mail != nullptr) {
        std::cout << "[DEBUG] get a mail " << maybe_mail->to_string() << std::endl;
        maybe_mail->run();
    }

    return m_mailbox_loop_running;
}

void MailboxProcessor::all_actions_completed() {
    sendControlMail(std::make_shared<AllCompleteMail>(shared_from_this()), "poison mail");
}


void MailboxProcessor::sendControlMail(std::shared_ptr<Runnable> runnable, std::string descrption) {
    m_mailbox->put(std::make_shared<Mail>(runnable, descrption));
}