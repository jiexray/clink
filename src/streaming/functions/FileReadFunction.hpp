/**
 * Read from files.
 */
#pragma once
#include "FlatMapFunction.hpp"
#include "Constant.hpp"
#include <memory>
#include <fstream>


// FileReadFunction is a flatMapFunction, input ONE file_path, output multiple file lines
class FileReadFunction final : public FlatMapFunction<std::string, std::string> {
public:
    void                                    flat_map(std::shared_ptr<std::string> val, 
                                                     std::shared_ptr<Collector<std::string>> out) override{
        // Get the file stream
        std::ifstream stream(Constant::CLINK_BASE + *val);

        if(stream.fail()) {
            throw std::runtime_error("Cannot find file in Path: " + *val);
        }
        
        // Iteratively read the file lines, and output each line to collector
        std::string line;
        while (std::getline(stream, line)) {
            out->collect(std::make_shared<std::string>(line));
        }
    }

    char*                                   serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<std::string, std::string>>  
                                            deserialize() override {return std::make_shared<FileReadFunction>();}
};
