#pragma once
#include "Trigger.hpp"
#include "TemplateHelper.hpp"
#include "TimeWindow.hpp"

/**
  A Trigger that fires once the current system time passes the end of the window
  to which a pane belongs.
 */
class ProcessingTimeTrigger: public Trigger<NullType, TimeWindow>
{
private:
    
public:
    ProcessingTimeTrigger() {

    }

    TriggerResult on_element(NullType* element, long timestamp, TimeWindow& window, TriggerContext& ctx) override {
        ctx.register_processing_time_timer(window.max_timestamp());
        return TriggerResult::CONTINUE;
    }

    TriggerResult on_processing_time(long time, TimeWindow& window, TriggerContext& ctx) override {
        return TriggerResult::FIRE;
    }

    TriggerResult on_event_time(long time, TimeWindow& window, TriggerResult& ctx) override {
        return TriggerResult::CONTINUE;
    }

    void clear(TimeWindow& window, TriggerContext& ctx) override {
        ctx.delete_processing_time_timer(window.max_timestamp());
    }

    std::string to_string() override {
        return "ProcessingTimeTrigger()";
    }
};

