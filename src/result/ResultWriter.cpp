#include "ResultWriter.hpp"

template<class T>
ResultWriter<T>::ResultWriter(std::shared_ptr<ResultPartition> result_partition, std::string task_name):
m_target_result_partition(result_partition),
m_task_name(task_name),
m_number_of_channels(result_partition->get_number_of_subpartitions()),
m_buffer_builders(m_number_of_channels, nullptr) {
    this->m_record_serializer = std::make_shared<StreamRecordSerializer<T>>();
}

/**
 * Copy the record to the buffer_builder.
 */
template <class T>
void ResultWriter<T>::copy_to_buffer_builder(int target_channel, std::shared_ptr<StreamRecord<T>> record) {
    std::shared_ptr<BufferBuilder> buffer_builder = get_buffer_builder(target_channel);
    // StreamRecordAppendResult result = record->serialize_record_to_buffer_builder(buffer_builder);
    StreamRecordAppendResult result = this->m_record_serializer->serialize(record, buffer_builder, true);

    while (result == FULL_RECORD_BUFFER_FULL || result == PARTITAL_RECORD_BUFFER_FULL || result == NONE_RECORD) {
        if (result == FULL_RECORD_BUFFER_FULL || result == NONE_RECORD) {
            // Full record has been copied to buffer, and the buffer is full.
            // Free the currrent buffer builder, and the buffer consumer in the result subpartition (target_channel) 
            // will take care of the buffer in the release buffer builder.

            // TODO: recycle the buffer builder
            // m_buffer_builders[target_channel].reset();
            if (result == FULL_RECORD_BUFFER_FULL)
                break;
        }
        buffer_builder = request_new_buffer_builder(target_channel);
        if (result == NONE_RECORD) {
            result = this->m_record_serializer->serialize(record, buffer_builder, true);
        } else {
            result = this->m_record_serializer->serialize(record, buffer_builder, false);
        }
    }

    // TODO: setup a flusher, current always flush
    flush(target_channel);
}

/**
 * Copy the record to the BufferBuilder of the target_channel.
 */
template <class T>
void ResultWriter<T>::emit(std::shared_ptr<StreamRecord<T>> record, int target_channel) {
    copy_to_buffer_builder(target_channel, record);
}

template <class T>
std::shared_ptr<BufferBuilder> ResultWriter<T>::get_buffer_builder(int target_channel) {
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
std::shared_ptr<BufferBuilder> ResultWriter<T>::request_new_buffer_builder(int target_channel) {
    std::shared_ptr<BufferBuilder> old_buffer_builder = m_buffer_builders[target_channel];

    std::shared_ptr<BufferBuilder> builder = m_target_result_partition->try_get_buffer_builder();
    if (builder == nullptr) {
        m_target_result_partition->get_buffer_builder();
    }
    m_target_result_partition->add_buffer_consumer(builder->create_buffer_consumer(), target_channel);
    m_buffer_builders[target_channel] = builder;
    return builder;
}

template class ResultWriter<std::string>;
