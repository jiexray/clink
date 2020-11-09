/**
 * A FileOutputFormat that writes objects to a text file.
 */
#pragma once

#include "FileOutputFormat.hpp"
#include "IOUtils.hpp"


template <class IT>
class TextOutputFormat: public FileOutputFormat<IT>
{
private:

public:
    TextOutputFormat(std::string output_path): FileOutputFormat<IT>(output_path){}

    ~TextOutputFormat() {
    }

    void write_record(IT* record) {
        std::string str_record = IOUtils::to_string<IT>(*record);
        this->m_stream << str_record;
        this->m_stream << "\n";
        this->m_stream.flush();
    }
};

