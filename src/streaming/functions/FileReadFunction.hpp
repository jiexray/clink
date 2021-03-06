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
#include <sstream>

// FileReadFunction is a flatMapFunction, input ONE file_path, output multiple file lines
class FileReadFunction final : public FlatMapFunction<StringBuf<128>, StringBuf<128>> {
public:
    void                                    flat_map(StringBuf<128>* val, 
                                                     std::shared_ptr<Collector<StringBuf<128>>> out) override{
        // Get the file stream
        std::ifstream stream(Constant::CLINK_BASE + std::string(val->c_str));

        int word_count = 0;
        if(stream.fail()) {
            throw std::runtime_error("Cannot find file in Path: " + std::string(val->c_str));
        }
        
        // Iteratively read the file lines, and output each line to collector
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream iss(line);
            
            std::string word;
            while(iss >> word) {
                word_count++;
                if (word_count % 1000000 == 0) {
                     std::cout << "have processed: " << word_count << " words" << std::endl;
                }
                StringBuf<128> new_word(word.c_str());
                out->collect(&new_word);
            }
        }
        std::cout << "finish reading file: " << std::string(val->c_str) << std::endl;
    }

    char*                                   serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<StringBuf<128>, StringBuf<128>>>  
                                            deserialize() override {return std::make_shared<FileReadFunction>();}
};
