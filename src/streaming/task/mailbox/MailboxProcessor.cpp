#include "MailboxProcessor.hpp"

std::shared_ptr<spdlog::logger>  MailboxProcessor::m_logger = spdlog::get("MailboxProcessor") == nullptr ?
                                                                spdlog::basic_logger_mt("MailboxProcessor", Constant::get_log_file_name()):
                                                                spdlog::get("MailboxProcessor");

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
        SPDLOG_LOGGER_DEBUG(m_logger, "get a mail {}", maybe_mail->to_string());
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