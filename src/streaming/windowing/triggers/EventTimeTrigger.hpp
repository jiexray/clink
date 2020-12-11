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
public:
    EventTimeTrigger() {}

    TriggerResult on_element(T* element, long timestamp, const TimeWindow& window, TriggerContext& ctx) override{
        if (window.max_timestamp() <= ctx.get_current_watermark()) {
            return TriggerResult::FIRE_AND_PURGE;
        } else {
            ctx.register_event_time_timer(window.max_timestamp());
            return TriggerResult::CONTINUE;
        }
    }

    TriggerResult on_processing_time(long time, const TimeWindow& window, TriggerContext& ctx) override {
        return TriggerResult::CONTINUE;
    }

    TriggerResult on_event_time(long time, const TimeWindow& window, TriggerContext& ctx) override {
        // std::cout << "EventTimeTrigger::on_event_time(), time: " << time << ", window max_timestamp: " <<  window.max_timestamp() << std::endl;
        // return time == window.max_timestamp() ? TriggerResult::FIRE_AND_PURGE : TriggerResult::CONTINUE;
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

