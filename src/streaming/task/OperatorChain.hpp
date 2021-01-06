/**
 * The OperatorChain contains all operators that are executed as one chain within a single StreamTask.
 */
#pragma once 
#include <memory>
#include "ResultWriterOutput.hpp"
#include "StreamOperatorFactory.hpp"
#include "CountingOutput.hpp"
#include "Environment.hpp"
#include "StreamConfig.hpp"
#include "ProcessingTimeServiceImpl.hpp"
// template <class OUT> class StreamOperatorFactory;

template <class OUT>
class OperatorChain
{
private:
    typedef std::shared_ptr<Environment>        EnvironmentPtr;
    typedef std::shared_ptr<OperatorMetricGroup> OperatorMetricGroupPtr; 

    // Write record to result partition through result writer.
    std::shared_ptr<ResultWriter<OUT>>          m_result_writer;

    // the core operator for this operator chain.
    // TODO: implement real operator chain, which connect each other through a link between consequtive operators.
    std::shared_ptr<StreamOperator<OUT>>        m_core_operator;

    // Note: Due to the cross-reference issue, we cannot directly put OperatorMetricGroup to StreamOperator.
    //       We put it here to undirectly enable metrics module in m_core_operator.
    OperatorMetricGroupPtr                      m_core_operator_metric_group;

    // only one ResultWriter for each OperatorChain
    std::shared_ptr<ResultWriterOutput<OUT>>    m_stream_output;

public:
    OperatorChain(std::shared_ptr<ResultWriter<OUT>> result_writer, 
            std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory){
        throw std::runtime_error("OperatorChain() is deperacated");
    }

    OperatorChain(std::shared_ptr<ResultWriter<OUT>> result_writer, 
            std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory,
            OperatorMetricGroupPtr operator_metric_group,
            std::shared_ptr<TaskInfo> task_info,
            ExecutionConfig& execution_config,
            ProcessingTimeService& processing_time_service);

    std::shared_ptr<ResultWriterOutput<OUT>>    create_result_stream_output();

    void                                        flush_output() {this->m_stream_output->flush();}

    void                                        close() {m_core_operator->close();}

    /* Properties */
    std::shared_ptr<StreamOperator<OUT>>        get_head_operator() {return m_core_operator;}
    OperatorMetricGroupPtr                      get_head_operator_metric_group() {return m_core_operator_metric_group;}
};


template <class OUT>
inline OperatorChain<OUT>::OperatorChain(
        std::shared_ptr<ResultWriter<OUT>> result_writer, 
        std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory, 
        OperatorMetricGroupPtr operator_metric_group,
        std::shared_ptr<TaskInfo> task_info,
        ExecutionConfig& execution_config,
        ProcessingTimeService& processing_time_service):
        m_result_writer(result_writer) {
    this->m_stream_output = create_result_stream_output();

    this->m_core_operator_metric_group = operator_metric_group;
    // this->m_core_operator = operator_factory->create_stream_operator(std::make_shared<StreamOperatorParameters<OUT>>(containing_task, this->m_stream_output));
    this->m_core_operator = operator_factory->create_stream_operator(
            std::make_shared<StreamOperatorParameters<OUT>>(
                    std::make_shared<CountingOutput<OUT>>(
                        this->m_stream_output, 
                        this->m_core_operator_metric_group->get_IO_metric_group()->get_records_out_counter()),
                    processing_time_service,  // ProcessTimeService
                    execution_config,  // ExecutionConfig
                    task_info  )); // TaskInfo
    this->m_core_operator->open();
}

template <class OUT>
inline std::shared_ptr<ResultWriterOutput<OUT>> OperatorChain<OUT>::create_result_stream_output() {
    return std::make_shared<ResultWriterOutput<OUT>>(this->m_result_writer);
}
