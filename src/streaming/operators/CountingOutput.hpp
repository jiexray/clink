/**
 * Wrapper Output that updates metrics on the number of emitted elements.
 */
#pragma once
#include "Output.hpp"
#include "Counter.hpp"
#include <memory>

template <typename OUT>
class CountingOutput: public Output<OUT>
{
private:
    typedef std::shared_ptr<Output<OUT>>    OutputPtr;
    typedef std::shared_ptr<Counter>        CounterPtr;

    OutputPtr                               m_output;
    CounterPtr                              m_num_records_out;

public:
    CountingOutput(OutputPtr output, CounterPtr counter): m_output(output), m_num_records_out(counter) {}

    void                                    collect(std::shared_ptr<StreamRecord<OUT>> record) {
        m_num_records_out->inc();
        m_output->collect(record);
    }
};

