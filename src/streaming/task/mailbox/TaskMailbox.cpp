#include "TaskMailbox.hpp"

void TaskMailbox::put(std::shared_ptr<Mail> mail) {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_queue.push_back(mail);
}

std::shared_ptr<Mail> TaskMailbox::try_take() {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    if (m_queue.empty()) {
        return nullptr;
    } else {
        std::shared_ptr<Mail> mail = m_queue.front();
        m_queue.pop_front();
        return mail;
    }
}
