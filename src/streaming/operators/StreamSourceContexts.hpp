/**
 * Source contexts for various stream time characteristics.
 */
#pragma once
#include "../functions/SourceFunction.hpp"
#include "Output.hpp"

enum TimeCharacteristic {
    ProcessingTime,
    IngestionTime, 
    EventTime
};

template <class OUT> class NonTimestampContext;

class StreamSourceContexts
{
public:
    template <class OUT>
    static std::shared_ptr<SourceContext<OUT>> get_source_context(TimeCharacteristic time_characteristic, std::shared_ptr<Output<OUT>> output) {
        switch (time_characteristic)
        {
        case TimeCharacteristic::EventTime:
            throw std::runtime_error("Have not implement TimeCharacteristic::EventTime");
            break;
        case TimeCharacteristic::IngestionTime:
            throw std::runtime_error("Have not implement TimeCharacteristic::IngestionTime");
            break;
        case TimeCharacteristic::ProcessingTime:
            return std::make_shared<NonTimestampContext<OUT>>(output);
            break;
        default:
            throw std::runtime_error("No TimeCharacteristic");
        }
    }
};


template <class OUT>
class NonTimestampContext : public SourceContext<OUT>
{
private:
    std::shared_ptr<Output<OUT>>    m_output;
    // TODO: reuse an stream record, without instantiate a stream record every time.
public:
    NonTimestampContext(std::shared_ptr<Output<OUT>> output): m_output(output) {}
    void collect(std::shared_ptr<OUT> element) {
        std::shared_ptr<StreamRecord<OUT>> new_record = std::make_shared<StreamRecord<OUT>>(element);
        if (this->m_output == nullptr) {
            std::cout << "[ERROR]Output in SourceContext is null" << std::endl;
            return;
        }
        m_output->collect(new_record);
    }
};

