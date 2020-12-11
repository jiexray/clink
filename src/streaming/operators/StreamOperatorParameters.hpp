/**
 * Helper class to construct AbstractUdfStreamOperator. Wraps couple of internal parameters
 * to simplify for user construction of class extending AbstractUdfStreamOperator and to 
 * allow for backward compatible changes in the AbstractUdfStreamOperator.
 */

#pragma once
#include <memory>
#include "StreamOperator.hpp"
#include "Output.hpp"
#include "ProcessingTimeService.hpp"
#include "ExecutionConfig.hpp"
#include "StateBackend.hpp"
#include "TaskInfo.hpp"
#include "TaskKvStateRegistry.hpp"


template <class OUT>
class StreamOperatorParameters
{
private:
    std::shared_ptr<Output<OUT>>                            m_output;
    ProcessingTimeService&                                  m_processing_time_service;
    ExecutionConfig&                                        m_execution_config;
    std::shared_ptr<TaskInfo>                               m_task_info;

public:
    StreamOperatorParameters(
            std::shared_ptr<Output<OUT>> output, 
            ProcessingTimeService& processing_timer_service,
            ExecutionConfig& execution_config,
            std::shared_ptr<TaskInfo> task_info): 
            m_output(output),
            m_processing_time_service(processing_timer_service),
            m_execution_config(execution_config),
            m_task_info(task_info) {}

    /* Properties */
    std::shared_ptr<Output<OUT>>                            get_output() {return m_output;}

    ProcessingTimeService&                                  get_processing_time_service() {return m_processing_time_service;}

    ExecutionConfig&                                        get_execution_config() {return m_execution_config;}

    std::shared_ptr<TaskInfo>                               get_task_info() {return m_task_info;}
};

