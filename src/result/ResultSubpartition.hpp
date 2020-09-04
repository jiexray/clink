/**
 * ResultSubpartition receive output record from task.
 * 0. During initialization, response to the subpartition registration from a downstream Task;
 * 1. Write record to a specfic buffer area temporaily;
 * 2. Notify the downstream task's RecordReader to poll the records (in the buffer);
 */
#pragma once

#include "ResultSubpartitionInfo.hpp"
#include "ResultPartition.hpp"
#include "ResultSubpartitionView.hpp"
#include "../buffer/BufferConsumer.hpp"
#include "../buffer/BufferBase.hpp"
#include "SubpartitionAvailableListener.hpp"
#include <memory>
#include <vector>
#include <deque>
#include <mutex>
#include <algorithm>

class ResultPartition;
class ResultSubpartitionView;

class BufferAndBacklog
{
private:
    BufferBase*         m_buffer;
    bool                m_is_data_available;
    int                 m_buffers_in_backlog;

public:
    BufferAndBacklog(BufferBase* buffer, bool is_data_available, int buffer_in_backlog):
    m_buffer(buffer), m_is_data_available(is_data_available), m_buffers_in_backlog(buffer_in_backlog){}

    /* Properties */
    BufferBase*         get_buffer() {return m_buffer;}
    bool                get_data_available() {return m_is_data_available;}
    int                 get_buffers_in_backlog() {return m_buffers_in_backlog;}
};


class ResultSubpartition : public std::enable_shared_from_this<ResultSubpartition>
{
private:
    std::shared_ptr<ResultSubpartitionInfo>                 m_subpartition_info;
    std::shared_ptr<ResultPartition>                        m_parent;
    std::deque<std::shared_ptr<BufferConsumer>>             m_buffers;     // TODO: currently, buffer is only buffer not EVENT
    std::mutex                                              m_buffers_mtx; // mutex to protect accesses to m_buffers 

    std::shared_ptr<ResultSubpartitionView>                 m_read_view;  // The read view to consume this subpartition

    bool                                                    m_flush_requested; // Mark the start of a notification, can only be trigger to true
                                                                               // by calling flush()

    void                                                    notify_data_available();
public:
    ResultSubpartition(int index, std::shared_ptr<ResultPartition> parent);
    ~ResultSubpartition() {}
    void                                                    release();

    /* Add BufferConsumer */
    bool                                                    add(std::shared_ptr<BufferConsumer>);

    /* Poll data from m_buffers to outside*/
    std::shared_ptr<BufferAndBacklog>                       poll_buffer();

    /* Create view for this subpartition, return to downstream task's ResultReader */
    std::shared_ptr<ResultSubpartitionView>                 create_read_view(std::shared_ptr<SubpartitionAvailableListener>);

    /* flush subparition, notify downstream ResultReader to poll data from this subpartition */
    void                                                    flush();

    /* Functions get access to buffer bulider, should guard by m_buffers_mtx!*/
    bool                                                    should_notify_data_available();
    int                                                     get_number_of_finished_buffers();
    int                                                     get_buffers_in_backlog();

    /* Properties */
    std::shared_ptr<ResultSubpartitionInfo>                 get_result_subpartition_info() {return m_subpartition_info;}
    std::shared_ptr<ResultSubpartitionView>                 get_read_view() {return m_read_view;}
    bool                                                    get_flush_requested() {return m_flush_requested;}
};
