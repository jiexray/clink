#include "TaskMailbox.hpp"
std::shared_ptr<spdlog::logger>  TaskMailbox::m_logger = LoggerFactory::get_logger("TaskMailbox");

void TaskMailbox::put(std::shared_ptr<Mail> mail) {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_queue.push_back(mail);
    has_new_mail = true;

    m_available_condition_variable.notify_one();
}

std::shared_ptr<Mail> TaskMailbox::try_take() {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    if (m_queue.empty()) {
        return nullptr;
    } else {
        std::shared_ptr<Mail> mail = m_queue.front();
        m_queue.pop_front();
        has_new_mail = !m_queue.empty();
        return mail;
    }
}

std::shared_ptr<Mail> TaskMailbox::take() {
    std::shared_ptr<Mail> mail = nullptr;

    mail = try_take(); 

    if (mail != nullptr) {
        return mail;
    }

    std::unique_lock<std::mutex> available_helper_lock(m_queue_mutex);

    // Important check: m_queue is empty, then wait
    m_available_condition_variable.wait(available_helper_lock, [this] {return !m_queue.empty();});

    mail = m_queue.front();
    m_queue.pop_front();

    has_new_mail = !m_queue.empty();
    return mail;
}