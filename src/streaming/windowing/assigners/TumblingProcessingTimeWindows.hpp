#pragma once
#include "WindowAssigner.hpp"
#include "TemplateHelper.hpp"
#include "TimeWindow.hpp"
#include "ProcessingTimeTrigger.hpp"

/**
  A WindowAssigner that windows elements into windows based on current 
  system time of the machine the operation is running on. Window cannot overlap.
 */
class TumblingProcessingTimeWindows: public WindowAssigner<NullType, TimeWindow>
{
private:
    long m_size;
    long m_offset;

public:
    TumblingProcessingTimeWindows(long size, long offset = 0): m_size(size), m_offset(offset) {}

    std::vector<TimeWindow> assign_windows(
            NullType* element, long timestamp, 
            WindowAssignerContext& context) override {
        long now = context.get_current_processing_time();
        long start = TimeWindow::get_window_start_with_offset(now, m_offset, m_size);
        return std::vector<TimeWindow>{TimeWindow(start, start + m_size)};
    }

    WindowAssigner<NullType, TimeWindow>* get_default_trigger() override {
        return (WindowAssigner<NullType, TimeWindow>*)(new ProcessingTimeTrigger());
    }

    bool is_event_time() override {
        return false;
    }

    std::string to_string() {
        return "TumblingProcessingTimeWindows(" + std::to_string(m_size) + ")";
    }
};

