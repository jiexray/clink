/**
 * The abstract base class for all Rich Output formats that are file based.
 * Contains the logic to open/close the target file streams.
 */
#pragma once
#include "OutputFormat.hpp"
#include <string>
#include <fstream>

template <class IT>
class FileOutputFormat: public OutputFormat<IT>
{
private:

protected:
    std::string             m_output_file_path;
    std::ofstream           m_stream;
    std::string             m_actual_file_path;
public:
    FileOutputFormat(std::string output_path): m_output_file_path(output_path) {}

    /* Properties */
    std::string             get_output_file_path() {return m_output_file_path;}

    void                    open(int task_number, int num_tasks) override {
        if (task_number < 0 || num_tasks < 1) {
            throw std::invalid_argument("Invalid task_number or num_task");
        }

        // // local file system

        m_actual_file_path = (num_tasks > 1) ? m_output_file_path + "/" + std::to_string(task_number + 1) : m_output_file_path;

        // create output file
        m_stream.open(m_actual_file_path);
    }

    void                                close() {
        m_stream.close();
    }
};

