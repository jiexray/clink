#pragma once
#include "Trigger.hpp"
#include "TemplateHelper.hpp"
#include "TimeWindow.hpp"

/**
  A Trigger that fires once the watermark passes the end of the window to which a pane belongs.
 */
template <class T>
class EventTimeTrigger: public Trigger<T, TimeWindow> {
private:
    bool m_deduplication_timer_bitmap[100];
public:
    EventTimeTrigger() {
        // memset(m_deduplication_timer_bitmap, 0, sizeof(m_deduplication_timer_bitmap));
    }

    TriggerResult on_element(T* element, long timestamp, const TimeWindow& window, TriggerContext& ctx) override{
        
        if (window.max_timestamp() <= ctx.get_current_watermark()) {
            return TriggerResult::FIRE_AND_PURGE;
        } else {
            // std::cout << "On register_event_time_timer(), window: " << window.to_string() << std::endl;
            ctx.register_event_time_timer(window.max_timestamp());
            return TriggerResult::CONTINUE;
        }
    }

    bool before_element(const TimeWindow& window) override {
        long max_ts = window.max_timestamp();
        if (!m_deduplication_timer_bitmap[max_ts % sizeof(m_deduplication_timer_bitmap)]) {
            m_deduplication_timer_bitmap[max_ts % sizeof(m_deduplication_timer_bitmap)] = true;
            return true;
        } else {
            return false;
        }
    }

    TriggerResult on_processing_time(long time, const TimeWindow& window, TriggerContext& ctx) override {
        return TriggerResult::CONTINUE;
    }

    TriggerResult on_event_time(long time, const TimeWindow& window, TriggerContext& ctx) override {
        // std::cout << "EventTimeTrigger::on_event_time(), time: " << time << ", window max_timestamp: " <<  window.max_timestamp() << std::endl;
        // return time == window.max_timestamp() ? TriggerResult::FIRE_AND_PURGE : TriggerResult::CONTINUE;
        m_deduplication_timer_bitmap[time % sizeof(m_deduplication_timer_bitmap)] = false;
        return time >= window.max_timestamp() ? TriggerResult::FIRE_AND_PURGE : TriggerResult::CONTINUE;
    }

    void clear(TimeWindow& window, TriggerContext& ctx) override {
        ctx.delete_event_time_timer(window.max_timestamp());
    }

    std::string to_string() override {
        return "EventTimeTrigger()";
    }

    static Trigger<T, TimeWindow>* create() {
        return new EventTimeTrigger<T>();
    }
};

