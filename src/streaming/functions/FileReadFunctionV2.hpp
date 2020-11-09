/**
 * Read from files.
 */
#pragma once
#include "FlatMapFunction.hpp"
#include "Constant.hpp"
#include <memory>
#include <iostream>
#include <cstddef>
#include <string>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
// #include "Tuple2.hpp"
#include "Tuple2V2.hpp"
#include "StringBuf.hpp"

// FileReadFunction is a flatMapFunction, input ONE file_path, output multiple file lines
class FileReadFunctionV2 final : public FlatMapFunction<StringBuf<32>, Tuple2V2<const char*, int, 128, sizeof(int)>> {
public:
    void                                    flat_map(StringBuf<32>* val, 
                                                     std::shared_ptr<Collector<Tuple2V2<const char*, int, 128, sizeof(int)>>> out) override {

        std::string file_name(Constant::CLINK_BASE + std::string(val->c_str));
        boost::interprocess::file_mapping mapping(file_name.c_str(), boost::interprocess::read_only);
        boost::interprocess::mapped_region mapped_rgn(mapping, boost::interprocess::read_only);
        char const* const mmaped_data = static_cast<char*>(mapped_rgn.get_address());
        std::size_t const mmap_size = mapped_rgn.get_size();
        std::cout << "mapped file size: " << mmap_size << std::endl;

        std::filebuf fbuf;
        fbuf.open(file_name, std::ios_base::in);

        std::vector<char> vect(mmap_size, 0);
        fbuf.sgetn(&vect[0], std::streamsize(vect.size()));

        const char *mem = static_cast<char*>(&vect[0]);

        int word_count = 0;
        std::string word = "";
        for (int i = 0; i < vect.size(); i++) {
            if (vect[i] == '\n' || vect[i] == ' ') {
                if (word.size() > 0) {
                    word_count++;
                    if (word_count % 1000000 == 0) {
                        std::cout << "have processed: " << word_count << " words" << std::endl;
                    }
                    // std::cout << word << std::endl;
                    int cnt = 1;
                    Tuple2V2<const char*, int, 128, sizeof(int)>* new_record = new Tuple2V2<const char*, int, 128, sizeof(int)>(word.c_str(), &cnt);
                    out->collect(new_record);
                    delete new_record;
                }
                word = "";
            } else {
                word += vect[i];
            }
        }
        std::cout << "finish reading file: " << file_name << std::endl;
    }

    char*                                   serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<StringBuf<32>, Tuple2V2<const char*, int, 128, sizeof(int)>>>  
                                            deserialize() override {return std::make_shared<FileReadFunctionV2>();}
};
