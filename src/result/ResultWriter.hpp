/**
 * Write results (records) to ResultPartition.
 */
#pragma once
#include "ResultPartition.hpp"
#include "../buffer/BufferBuilder.hpp"
#include "../streamrecord/StreamRecord.hpp"
#include "../streamrecord/StreamRecordSerializer.hpp"
#include "ChannelSelector.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <string>
#include "Tuple.hpp"

template <class T>
class ResultWriter
{
private:
    std::shared_ptr<ResultPartition>            m_target_result_partition;
    int                                         m_number_of_channels;
    std::string                                 m_task_name;
    std::vector<std::shared_ptr<BufferBuilder>> m_buffer_builders;
    std::shared_ptr<StreamRecordSerializer<T>>  m_record_serializer; 

    static std::shared_ptr<spdlog::logger>      m_logger;   

public:
    ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name);

    /* Put the record to the ResultPartition */
    void                                        emit(std::shared_ptr<StreamRecord<T>> record, int target_channel);
    virtual void                                emit(std::shared_ptr<StreamRecord<T>> record) {}
    void                                        copy_to_buffer_builder(int target_channel, std::shared_ptr<StreamRecord<T>> record);

    /* Request BufferBuilder from BufferPool, add BufferConsumer to subpartition */
    std::shared_ptr<BufferBuilder>              get_buffer_builder(int target_channel);
    std::shared_ptr<BufferBuilder>              request_new_buffer_builder(int target_channel);
    
    /* Flush */
    void                                        flush_all() {m_target_result_partition->flush_all();}
    void                                        flush(int subpartition_idx) {m_target_result_partition->flush(subpartition_idx);}

    /* Properties */
    int                                         get_number_of_channels() {return m_number_of_channels;}
};
