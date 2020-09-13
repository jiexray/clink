/**
 * ResultPartition manage all result subpartitions. Each task has its onw ResultPartition,
 * and each subpartition in it is connected to a downstream task or a sink.
 */
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "ResultSubpartition.hpp"
#include "ResultSubpartitionView.hpp"
#include "../buffer/BufferPool.hpp"
#include "../buffer/BufferBuilder.hpp"

class ResultSubpartition;
class ResultSubpartitionView;
class SubpartitionAvailableListener;

class ResultPartition : public std::enable_shared_from_this<ResultPartition>
{
private:
    std::string                                         m_owning_task_name;
    int                                                 m_partition_idx;

    std::vector<std::shared_ptr<ResultSubpartition>>    m_subpartitions;

    std::shared_ptr<BufferPool>                         m_buffer_pool;

public:
    ResultPartition(std::string, int, int, std::shared_ptr<BufferPool>);
    void                                                setup(); // setup subpartitions array

    /* Properties */
    int                                                 get_partition_idx() {return m_partition_idx;}
    std::shared_ptr<ResultSubpartition>                 get_subpartition(int subpartition_idx);
    int                                                 get_number_of_subpartitions() {return (int) m_subpartitions.size();}
    std::string                                         get_owning_task_name() {return m_owning_task_name;}

    // partition_id is the unique identification of the ResultPartition, use this to register / request ResultPartition
    std::string                                         get_partition_id() {return m_owning_task_name + "-" + std::to_string(m_partition_idx);}
    

    /* Request BufferBuilder from buffer pool / Add BufferConsumer to subpartition */ 
    std::shared_ptr<BufferBuilder>                      get_buffer_builder();
    std::shared_ptr<BufferBuilder>                      try_get_buffer_builder();
    bool                                                add_buffer_consumer(std::shared_ptr<BufferConsumer>, int);

    /* create view / flush subpartitions */
    std::shared_ptr<ResultSubpartitionView>             create_subpartition_view(int, std::shared_ptr<SubpartitionAvailableListener>);
    void                                                flush(int);
    void                                                flush_all();

};

