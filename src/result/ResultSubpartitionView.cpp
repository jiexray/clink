#include "ResultSubpartitionView.hpp"

std::shared_ptr<BufferAndBacklog> ResultSubpartitionView::get_next_buffer() {
    return m_parent->poll_buffer();
}

void ResultSubpartitionView::notify_data_available() {
    m_available_listener->notify_data_available();
}