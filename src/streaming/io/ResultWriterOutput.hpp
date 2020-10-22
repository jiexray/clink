/**
 * Implementation of Output (in operators) that sends data using ResultWriter
 */
#pragma once

#include "../operators/Output.hpp"
#include "../../result/ResultWriter.hpp"
#include <memory>

template <class OUT>
class ResultWriterOutput : public Output<OUT>
{
private:
    std::shared_ptr<ResultWriter<OUT>>       m_result_writer;


public:
    ResultWriterOutput(std::shared_ptr<ResultWriter<OUT>> result_writer): m_result_writer(result_writer) {}


    /* implement interface from Output */
    void                                    collect(std::shared_ptr<StreamRecord<OUT>> record) {
        // std::cout << "ResultWriterOutput::collect(), output record: " << record->to_string() << std::endl;
        this->m_result_writer->emit(record);
    }
    void                                    flush() {this->m_result_writer->flush_all();}
};

