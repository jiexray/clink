/**
 * Input reader for OneInputStreamTask. Receiving data from source or network.
 */
#pragma once
#include "StreamTaskInput.hpp"
#include "DataOutput.hpp"
#include "InputStatus.hpp"
#include "StreamInputProcessor.hpp"
#include <memory>

template <class IN>
class StreamOneInputProcessor : public StreamInputProcessor
{
private:
    std::shared_ptr<StreamTaskInput<IN>>        m_input;
    std::shared_ptr<DataOutput<IN>>             m_output;
public:
    StreamOneInputProcessor(std::shared_ptr<StreamTaskInput<IN>> input, std::shared_ptr<DataOutput<IN>> output):
    m_input(input), m_output(output) {}
    
    InputStatus                                 process_input() override { return m_input->emit_next(m_output);}
};
