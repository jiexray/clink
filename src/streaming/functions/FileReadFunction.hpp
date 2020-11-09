/**
 * Read from files.
 */
#pragma once
#include "FlatMapFunction.hpp"
#include "Constant.hpp"
#include "StringBuf.hpp"
#include <memory>
#include <fstream>
#include <iostream>

// FileReadFunction is a flatMapFunction, input ONE file_path, output multiple file lines
class FileReadFunction final : public FlatMapFunction<StringBuf<1024>, StringBuf<1024>> {
public:
    void                                    flat_map(StringBuf<1024>* val, 
                                                     std::shared_ptr<Collector<StringBuf<1024>>> out) override{
        // Get the file stream
        std::ifstream stream(Constant::CLINK_BASE + std::string(val->c_str));

        if(stream.fail()) {
            throw std::runtime_error("Cannot find file in Path: " + std::string(val->c_str));
        }
        
        // Iteratively read the file lines, and output each line to collector
        std::string line;
        while (std::getline(stream, line)) {
            StringBuf<1024> new_line(line.c_str());
            out->collect(&new_line);
        }
        std::cout << "finish reading file: " << std::string(val->c_str) << std::endl;
    }

    char*                                   serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<StringBuf<1024>, StringBuf<1024>>>  
                                            deserialize() override {return std::make_shared<FileReadFunction>();}
};
