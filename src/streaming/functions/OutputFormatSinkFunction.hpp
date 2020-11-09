/**
 * Simple implementation of SinkFunction writing tuples to file
 */
#pragma once
#include "SinkFunction.hpp"
#include "OutputFormat.hpp"
#include "FileOutputFormat.hpp"
#include "TextOutputFormat.hpp"

#include <memory>


template <class IN>
class OutputFormatSinkFunction: public SinkFunction<IN>
{
private:
    std::shared_ptr<OutputFormat<IN>>       m_format;
    std::string                             m_output_file_path;
public:
    OutputFormatSinkFunction(std::shared_ptr<OutputFormat<IN>> format): m_format(format){}

    /* Properties */
    void                                    set_output_file_path(std::string output_file_path){
        this->m_output_file_path = output_file_path;
    }

    void                                    open() {
        // TODO: get RuntimeContext
        this->m_format->open(0, 1);
    }

    void                                    close() {
        this->m_format->close();
    }

    void                                    invoke(IN* value) {
        this->m_format->write_record(value);
    }

    char*                                   serialize() {
        this->m_output_file_path = std::dynamic_pointer_cast<FileOutputFormat<IN>>(m_format)->get_output_file_path();
        return (char*)this;
    }

    std::shared_ptr<SinkFunction<IN>>       deserialize(char* des_type) {
        OutputFormatSinkFunction<IN>* output_format_sink_function_ptr = (OutputFormatSinkFunction<IN>*)(des_type);
        std::shared_ptr<OutputFormat<IN>> format = std::make_shared<TextOutputFormat<IN>>(output_format_sink_function_ptr->m_output_file_path);
        return std::make_shared<OutputFormatSinkFunction<IN>>(format);
    }
};

