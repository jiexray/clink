#pragma once

#include "cxxtest/TestSuite.h"
#include "TextOutputFormat.hpp"
#include "Constant.hpp"

#include <memory>


class TestFileOutputFormat : public CxxTest::TestSuite
{
public:
    void testTextFileFormat( void ) {
        std::shared_ptr<TextOutputFormat<int>> int_text_output_format = std::make_shared<TextOutputFormat<int>>(Constant::CLINK_BASE + "/resource/test_write_int.txt"); 
        int_text_output_format->open(0, 1);
        int i_val = 100;
        int_text_output_format->write_record(i_val);
        int_text_output_format->close();

        std::shared_ptr<TextOutputFormat<Tuple2<std::string, int>>> tuple_text_output_format = 
                        std::make_shared<TextOutputFormat<Tuple2<std::string, int>>>(Constant::CLINK_BASE + "/resource/test_write_tuple.txt"); 
        
        tuple_text_output_format->open(0, 1);
        Tuple2<std::string, int> tuple(std::make_shared<std::string>("hello world"), std::make_shared<int>(1000));
        tuple_text_output_format->write_record(tuple);

        // std::shared_ptr<TextOutputFormat<std::string>> string_text_output_format = std::make_shared<TextOutputFormat<std::string>>(Constant::CLINK_BASE + "/resource/test_write_string.txt"); 
        // int_text_output_format->open(0, 1);
        // int i_val = 100;
        // int_text_output_format->write_record(i_val);
        // int_text_output_format->close();
    }
};
