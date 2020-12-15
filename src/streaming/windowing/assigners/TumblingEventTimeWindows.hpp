#pragma once
#include "WindowAssigner.hpp"
#include "TemplateHelper.hpp"
#include "TimeWindow.hpp"
#include "EventTimeTrigger.hpp"
#include <vector>

/**
  A WindowAssigner that windows elements into windows based on the timestap of the 
  elements. Windows cannot overlaps.
 */
template <class T>
class TumblingEventTimeWindows: public WindowAssigner<T, TimeWindow> {
private:
    long m_size;
    long m_offset;

    std::vector<TimeWindow> m_windows;
public:
    TumblingEventTimeWindows(long size, long offset): m_size(size), m_offset(offset) {

    }

    void assign_windows(T* element, long timestamp, WindowAssignerContext& context) override {
        long start = TimeWindow::get_window_start_with_offset(timestamp, m_offset, m_size);
        if (m_windows.empty()) {
            m_windows.emplace_back(start, start + m_size);
        } else {
            m_windows[0].set_start(start);
            m_windows[0].set_end(start + m_size);
        }
    }

    const std::vector<TimeWindow>& get_assigned_windows() {
        return m_windows;
    }

    Trigger<T, TimeWindow>* get_default_trigger() {
        return new EventTimeTrigger<T>();
    }

    bool is_event_time() override {
        return true;
    }

    std::string to_string() {
        return "TumblingEventTimeWindows(" + std::to_string(m_size) + ")";
    }

    static TumblingEventTimeWindows<T>* create(long size, long offset) {
        return new TumblingEventTimeWindows<T>(size, offset);
    }
};
