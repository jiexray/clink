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

    int _current_ts = -1;
    long _mid_ts_counter = 0;
    long _total_counter = 0;
public:
    TumblingEventTimeWindows(long size, long offset): m_size(size), m_offset(offset) {

    }

    std::vector<TimeWindow> assign_windows(T* element, long timestamp, WindowAssignerContext& context) override {
        // _total_counter++;
        // if (timestamp % m_size == _current_ts) {
        //     _mid_ts_counter++;
        // } else {
        //     if (_current_ts != -1) {
        //         std::cout << "timestamp " << _current_ts << " emit " << _mid_ts_counter << " records, total " << _total_counter << std::endl;
        //     } 
        //     _mid_ts_counter = 0;
        //     _current_ts = timestamp % m_size;
        // }


        long start = TimeWindow::get_window_start_with_offset(timestamp, m_offset, m_size);
        return std::vector<TimeWindow>{TimeWindow(start, start + m_size)};
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
