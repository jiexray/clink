#pragma once
#include "WindowAssigner.hpp"
#include "TemplateHelper.hpp"
#include "TimeWindow.hpp"
#include "ProcessingTimeTrigger.hpp"

/**
  A WindowAssigner that windows elements into windows based on current 
  system time of the machine the operation is running on. Window cannot overlap.
 */
template <class T>
class TumblingProcessingTimeWindows: public WindowAssigner<T, TimeWindow>
{
private:
    long m_size;
    long m_offset;

public:
    /**
      Create a tumbling processing-time window.

      @param size The size of a tumbling window, in ms
      @param offset The offset of the first window
     */
    TumblingProcessingTimeWindows(long size, long offset = 0): m_size(size), m_offset(offset) {}

    std::vector<TimeWindow> assign_windows(
            T* element, long timestamp, 
            WindowAssignerContext& context) override {
        long now = context.get_current_processing_time();
        long start = TimeWindow::get_window_start_with_offset(now, m_offset, m_size);
        return std::vector<TimeWindow>{TimeWindow(start, start + m_size)};
    }

    Trigger<T, TimeWindow>* get_default_trigger() override {
        return (Trigger<T, TimeWindow>*)(new ProcessingTimeTrigger<T>());
    }

    bool is_event_time() override {
        return false;
    }

    std::string to_string() {
        return "TumblingProcessingTimeWindows(" + std::to_string(m_size) + ")";
    }

    static TumblingProcessingTimeWindows<T>* create(long size, long offset) {
        return new TumblingProcessingTimeWindows<T>(size, offset);
    }
};

