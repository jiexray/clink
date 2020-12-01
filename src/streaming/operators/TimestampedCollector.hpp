#pragma once
#include "Output.hpp"
#include <memory>

/**
  Wrapper around Output for user functions that expect a Output.
  Before giving the TimestampedCollector to a user function you must set
  the timestamp that should be attached to emitted elements. Most operators
  would set the timestamp of the incoming StreamRecord here.

  @param <T> The type of the elements that can be emitted.
 */
template <class T>
class TimestampedCollector: public Output<T>
{
private:
    std::shared_ptr<Output<T>> m_output;

    StreamRecordV2<T>* m_reuse;
public:
    TimestampedCollector(std::shared_ptr<Output<T>> output):
            m_output(output) {
        m_reuse = new StreamRecordV2<T>();
    }

    ~TimestampedCollector() {
        delete m_reuse;
    }

    void collect(T* val) override {
        m_reuse->val = *val;
        m_output->collect(m_reuse);
    }

    void collect(StreamRecordV2<T>* record) {
        m_output->collect(record);
    }

    void set_timestamp(long timestamp) {
        m_reuse->timestamp = timestamp;
    }

    void erase_timestamp() {
        m_reuse->timestamp = -1;
    }

    void emit_watermark(StreamRecordV2<T>* mark) override {
        m_output->emit_watermark(mark);
    }
};
