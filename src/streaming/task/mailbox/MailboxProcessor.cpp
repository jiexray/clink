#include "MailboxProcessor.hpp"

std::shared_ptr<spdlog::logger>  MailboxProcessor::m_logger = LoggerFactory::get_logger("MailboxProcessor");

void MailboxProcessor::run_mailbox_loop() {
    std::shared_ptr<MailboxDefaultAction::Controller> default_action_context = std::make_shared<MailboxController>(shared_from_this());
    while(run_mailbox_step(default_action_context)){}
}

bool MailboxProcessor::run_mailbox_step(std::shared_ptr<MailboxDefaultAction::Controller> default_action_context) {
    if (process_mail()) {
        m_mailbox_default_action->run_default_action(default_action_context);
        return true;
    } else {
        return false;
    }
}

bool MailboxProcessor::process_mail() {
    // take mails in a non-blockingly and execute them.
    std::shared_ptr<Mail> maybe_mail;
    while (m_mailbox_loop_running && (maybe_mail = m_mailbox->try_take()) != nullptr) {
        SPDLOG_LOGGER_TRACE(m_logger, "get a mail {}", maybe_mail->to_string());
        maybe_mail->run();
    }

    // If the default action is currently not available, we can run a blocking mailbox execution until the default
    // action becomes available again.
    while(m_mailbox_loop_running && is_default_action_unavailable()) {
        maybe_mail = m_mailbox->try_take();
        if (maybe_mail == nullptr) {
            auto start = std::chrono::steady_clock::now();
            maybe_mail = m_mailbox->take();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> waiting_time = end - start;
            m_idle_time->mark_event((long)(waiting_time.count() * 1000));
        }

        if (maybe_mail == nullptr) {
            throw std::runtime_error("cannot get null Mail using blocking Mailbox::take()");
        }
        maybe_mail->run();
    }


    return m_mailbox_loop_running;
}

void MailboxProcessor::all_actions_completed() {
    sendControlMail(std::bind(&MailboxProcessor::complete, this), "poison mail");
}
