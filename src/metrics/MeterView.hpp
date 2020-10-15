/**
 * A MeterView provides an average rate of events per second over a given time period.
 */
#pragma once
#include "Metric.hpp"
#include "Counter.hpp"
#include "Meter.hpp"
#include "View.hpp"
#include "SimpleCounter.hpp"
#include <cmath>
#include <memory>
#include <iostream>
#include <cstring>

class MeterView: public Meter, public View
{
private:
    static int                      DEFAULT_TIME_SPAN_IN_SECONDS;
    /** The underlying counter maintaining the count */
    std::shared_ptr<Counter>        m_counter;
    /** The time-span over which the average is calculated */
    int                             m_time_span_in_seconds;
    /** Circular array containing the history of values */
    long*                           m_values;
    int                             m_values_length;
    /** The index in the array for the currenttime */
    int                             m_time; 
    /** The last rate we computed */
    double                          m_current_rate;
public:
    MeterView(std::shared_ptr<Counter> counter, int time_span_in_seconds) {
        m_time = 0;
        m_current_rate = 0.0;
        m_counter = counter;
        m_time_span_in_seconds = std::max(time_span_in_seconds - (time_span_in_seconds % UPDATE_INTERVAL_SECONDS),
                                            UPDATE_INTERVAL_SECONDS);
        m_values_length = m_time_span_in_seconds / UPDATE_INTERVAL_SECONDS + 1;
        m_values = new long[m_values_length];
        memset(m_values, 0, m_values_length * sizeof(long));
    }

    MeterView(std::shared_ptr<Counter> counter): MeterView(counter, DEFAULT_TIME_SPAN_IN_SECONDS) {}

    MeterView(int time_span_in_seconds): MeterView(std::make_shared<SimpleCounter>(), time_span_in_seconds) {}

    ~MeterView() {
        delete[] m_values;
    }

    /* override functions from meter */
    void                            mark_event() {m_counter->inc();}
    void                            mark_event(long n) {m_counter->inc(n);}
    long                            get_count() {return m_counter->get_count();}
    double                          get_rate() {return m_current_rate;}

    void                            update() {
        m_time = (m_time + 1) % (m_values_length);
        m_values[m_time] = m_counter->get_count();
        m_current_rate = ((double) (m_values[m_time] - m_values[(m_time + 1) % m_values_length])) / (double)m_time_span_in_seconds;
    }

};
