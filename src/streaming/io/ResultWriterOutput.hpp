/**
 * Implementation of Output (in operators) that sends data using ResultWriter
 */
#pragma once

#include "Output.hpp"
#include "ResultWriter.hpp"
#include "StreamRecordV2.hpp"
#include <memory>
#include <assert.h>

template <class OUT>
class ResultWriterOutput : public Output<OUT>
{
private:
    std::shared_ptr<ResultWriter<OUT>>       m_result_writer;


public:
    ResultWriterOutput(std::shared_ptr<ResultWriter<OUT>> result_writer): m_result_writer(result_writer) {}


    /* implement interface from Output */
    void                                    collect(StreamRecordV2<OUT>* record) override {
        // std::cout << "ResultWriterOutput::collect(), output record: " << record->to_string() << std::endl;
        this->m_result_writer->emit(record);
    }

    void                                    flush() {
        this->m_result_writer->flush_all();
    }

    void                                    emit_watermark(StreamRecordV2<OUT>* mark) override {
        assert(mark->type == StreamRecordType::WATERMARK);
        this->m_result_writer->emit(mark);
    }
};

