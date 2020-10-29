/**
 * Write results (records) to ResultPartition.
 */
#pragma once
#include "ResultPartition.hpp"
#include "BufferBuilder.hpp"
#include "StreamRecord.hpp"
#include "StreamRecordSerializer.hpp"
#include "ChannelSelector.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <string>
#include "Tuple.hpp"
#include "Counter.hpp"
#include "Meter.hpp"
#include "MetricGroup.hpp"
#include "TaskIOMetricGroup.hpp"

template <class T>
class OutputFlusher;

template <class T>
class ResultWriter: public std::enable_shared_from_this<ResultWriter<T>>
{
    typedef std::shared_ptr<Counter>            CounterPtr;
    typedef std::shared_ptr<Meter>              MeterPtr;
    typedef std::shared_ptr<TaskIOMetricGroup>  TaskIOMetricGroupPtr;
public:
    ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name);

    ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name, long timeout);

    void                                        setup();

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
    std::shared_ptr<spdlog::logger>             get_logger() {return m_logger;}

    /* Metrics */
    void                                        set_metric_group(TaskIOMetricGroupPtr metrics) {
        m_bytes_out = metrics->get_bytes_out_counter();
        m_buffers_out = metrics->get_buffers_out_counter();
        m_idle_time_ms_per_second = metrics->get_idle_time_ms_per_second();
    }

    void                                        finish_buffer_builder(std::shared_ptr<BufferBuilder> buffer_builder) {
        m_bytes_out->inc(buffer_builder->finish());
        m_buffers_out->inc();
    }
private:
    std::shared_ptr<ResultPartition>            m_target_result_partition;
    int                                         m_number_of_channels;
    std::string                                 m_task_name;
    std::vector<std::shared_ptr<BufferBuilder>> m_buffer_builders;
    std::shared_ptr<StreamRecordSerializer<T>>  m_record_serializer; 
    bool                                        m_always_flush;
    long                                        m_flush_timeout;

    std::shared_ptr<spdlog::logger>             m_logger;
    OutputFlusher<T>*                           m_output_flusher;

    /* Metrics indices */
    CounterPtr                                  m_bytes_out;
    CounterPtr                                  m_buffers_out;
    MeterPtr                                    m_idle_time_ms_per_second;
};

template <class T>
class OutputFlusher {
public:
    OutputFlusher(const std::string& name, long timeout, std::shared_ptr<ResultWriter<T>> writer) {
        m_timeout = timeout;
        m_writer = writer;
    }

    void terminate() {
        m_running = false;
    }

    void run () {
        while(m_running) {
            SPDLOG_LOGGER_DEBUG(m_writer->get_logger(), "flush ResultWriter");
            std::this_thread::sleep_for(std::chrono::milliseconds(m_timeout));

            m_writer->flush_all();
        }
    }

    void start() {
        std::thread m_worker_thread = std::thread(std::bind(&OutputFlusher::run, this));
        m_worker_thread.detach();
    }
private:
    long                m_timeout;
    volatile bool       m_running = true;

    std::shared_ptr<ResultWriter<T>> m_writer;
};

template<class T>
inline ResultWriter<T>::ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name, long timeout):
m_target_result_partition(result_partition),
m_task_name(task_name),
m_number_of_channels(result_partition->get_number_of_subpartitions()),
m_buffer_builders(m_number_of_channels, nullptr) {
    this->m_record_serializer = std::make_shared<StreamRecordSerializer<T>>();
    m_logger = LoggerFactory::get_logger("ResultWriter");

    if (timeout == 0) {
        m_always_flush = true;
        m_flush_timeout = -1;
    } else {
        m_always_flush = false;
        m_flush_timeout = timeout;
    }
}

template<class T>
inline ResultWriter<T>::ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name): 
ResultWriter(result_partition, task_name, 0) {}

template<class T>
inline void ResultWriter<T>::setup() {
    if (!m_always_flush) {
        m_output_flusher = new OutputFlusher<T>("OutputFlusher for " + m_task_name, m_flush_timeout, this->shared_from_this());
        m_output_flusher->start();
    }
}

/**
 * Copy the record to the buffer_builder.
 */
template <class T>
inline void ResultWriter<T>::copy_to_buffer_builder(int target_channel, std::shared_ptr<StreamRecord<T>> record) {
    int num_copied_buffers = 0;
    std::shared_ptr<BufferBuilder> buffer_builder = get_buffer_builder(target_channel);
    SPDLOG_LOGGER_TRACE(m_logger, "ResultWriter<T>::copy_to_buffer_builder() after get builder with Buffer {}, current write position {}, buffer_size {}", 
                                        buffer_builder->get_buffer()->get_buffer_id(), buffer_builder->get_write_position(), buffer_builder->get_buffer()->get_max_capacity());
    // StreamRecordAppendResult result = record->serialize_record_to_buffer_builder(buffer_builder);
    StreamRecordAppendResult result = this->m_record_serializer->serialize(record, buffer_builder, true);
    SPDLOG_LOGGER_TRACE(m_logger, "ResultWriter<T>::copy_to_buffer_builder() after first serialize, result {}, current write position {}", 
                                        result, buffer_builder->get_write_position());

    if (result != NONE_RECORD) {
        num_copied_buffers++;
    }

    while (result == FULL_RECORD_BUFFER_FULL || result == PARTITAL_RECORD_BUFFER_FULL || result == NONE_RECORD) {
        finish_buffer_builder(buffer_builder);
        if (result == FULL_RECORD_BUFFER_FULL || result == NONE_RECORD) {
            // Full record has been copied to buffer, and the buffer is full.
            // Free the currrent buffer builder, and the buffer consumer in the result subpartition (target_channel) 
            // will take care of the buffer in the release buffer builder.

            // m_buffer_builders[target_channel].reset();
            if (result == FULL_RECORD_BUFFER_FULL)
                break;
        }
        if (num_copied_buffers == m_target_result_partition->get_buffer_pool_capacity()){
            SPDLOG_LOGGER_ERROR(m_logger, "One streamrecord span all Buffers, but still unfinished, cause request buffer stall");
        }
        buffer_builder = request_new_buffer_builder(target_channel);
        num_copied_buffers++;
        SPDLOG_LOGGER_TRACE(m_logger, "Write one record span {} Buffers, total number of Buffers: {}", num_copied_buffers,
                                m_target_result_partition->get_buffer_pool_capacity());
        if (result == NONE_RECORD) {
            // length is still not written
            result = this->m_record_serializer->serialize(record, buffer_builder, true);
        } else {
            result = this->m_record_serializer->serialize(record, buffer_builder, false);
        }
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Finish write one record, span {} Buffers", num_copied_buffers);    

    // TODO: setup a flusher, current always flush
    if (m_always_flush) {
        flush(target_channel);
    }
    SPDLOG_LOGGER_TRACE(m_logger, "Finish flush channel {}", target_channel); 
}

/**
 * Copy the record to the BufferBuilder of the target_channel.
 */
template <class T>
inline void ResultWriter<T>::emit(std::shared_ptr<StreamRecord<T>> record, int target_channel) {
    copy_to_buffer_builder(target_channel, record);
}

template <class T>
inline std::shared_ptr<BufferBuilder> ResultWriter<T>::get_buffer_builder(int target_channel) {
    if (m_buffer_builders[target_channel] != nullptr) {
        return m_buffer_builders[target_channel];
    } else {
        return request_new_buffer_builder(target_channel);
    }
}

/**
 * Request BufferBuilder from ResultPartition, and add its BufferConsumer to the corresponding subpartition (index == target_channel)
 */
template <class T>
inline std::shared_ptr<BufferBuilder> ResultWriter<T>::request_new_buffer_builder(int target_channel) {
    std::shared_ptr<BufferBuilder> old_buffer_builder = m_buffer_builders[target_channel];

    std::shared_ptr<BufferBuilder> builder = m_target_result_partition->try_get_buffer_builder();
    if (builder == nullptr) {
        builder = m_target_result_partition->get_buffer_builder();
    }
    m_target_result_partition->add_buffer_consumer(builder->create_buffer_consumer(), target_channel);
    m_buffer_builders[target_channel] = builder;
    return builder;
}