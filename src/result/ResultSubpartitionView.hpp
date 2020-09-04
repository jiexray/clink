/**
 * ResultSubparitionView is the bridge between ResultSubpartition and RecordReader.
 */
#pragma once
#include "ResultSubpartition.hpp"
#include "../buffer/BufferBase.hpp"
#include "SubpartitionAvailableListener.hpp"
#include <memory>
#include <iostream>

class ResultSubpartition;
class BufferAndBacklog;
class SubpartitionAvailableListener;

class ResultSubpartitionView
{
private:
    std::shared_ptr<ResultSubpartition>                 m_parent;
    std::shared_ptr<SubpartitionAvailableListener>      m_available_listener;
public:
    ResultSubpartitionView(std::shared_ptr<ResultSubpartition> parent, std::shared_ptr<SubpartitionAvailableListener> available_listener):
            m_parent(parent), m_available_listener(available_listener){}
    ~ResultSubpartitionView() {std::cout << "destroy ResultSubpartition" << std::endl;};

    /* Transfer data from this subpartition to downstream record reader */
    std::shared_ptr<BufferAndBacklog>                   get_next_buffer();

    /* Notify the downstream ResultReader */
    void                                                notify_data_available();
};
