#pragma once
#include "StreamOperatorFactory.hpp"
#include "WindowOperator.hpp"
#include "KeyGroupRangeAssignment.hpp"
#include <functional>
#include "TimeWindow.hpp"

/**
  StreamOperatorFactory for WindowOperator
 */
template<class K, class IN, class ACC, class OUT, class W = TimeWindow>
class WindowOperatorFactory: public StreamOperatorFactory<OUT>
{
private:
    typedef std::function<WindowAssigner<IN, W>*(void)> WindowAssignerCreator;
    WindowAssignerCreator m_window_assigner_creator;

    typedef std::function<std::shared_ptr<InternalWindowFunction<IN, OUT, K, W>>(void)> InternalWindowFunctionCreator;
    InternalWindowFunctionCreator m_internal_window_function_creator;

    typedef std::function<Trigger<IN, W>*(void)> TriggerCreator;
    TriggerCreator m_trigger_creator;

    typedef std::function<StateDescriptor<AppendingState<IN, ACC>, ACC>*(void)> StateDescriptorCreator;
    StateDescriptorCreator m_state_descriptor_creator;
    typedef std::function<InternalAppendingState<K, W, IN, ACC, ACC>*(const StateDescriptor<AppendingState<IN, ACC>, ACC> &, StateTable<K, W, ACC>&)> StateCreator;
    StateCreator m_state_creator;

    typedef AbstractKeyedStateBackend<K, W, ACC, AppendingState<IN, ACC>, InternalAppendingState<K, W, IN, ACC, ACC>> AbstractKeyedStateBackendType;
    StateBackend<K, W, ACC, AppendingState<IN, ACC>, InternalAppendingState<K, W, IN, ACC, ACC>>* m_state_backend;

public:

    WindowOperatorFactory(
        WindowAssignerCreator window_assigner_creator,
        InternalWindowFunctionCreator internal_window_function_creator,
        TriggerCreator trigger_creator,
        StateDescriptorCreator state_descriptor_creator,
        StateCreator state_creator):
        m_window_assigner_creator(window_assigner_creator),
        m_internal_window_function_creator(internal_window_function_creator),
        m_trigger_creator(trigger_creator),
        m_state_descriptor_creator(state_descriptor_creator),
        m_state_creator(state_creator) {}

    WindowOperatorFactory(const WindowOperatorFactory& other):
        m_window_assigner_creator(other.m_window_assigner_creator),
        m_internal_window_function_creator(other.m_internal_window_function_creator),
        m_trigger_creator(other.m_trigger_creator),
        m_state_descriptor_creator(other.m_state_descriptor_creator),
        m_state_creator(other.m_state_creator) {}

    void set_state_backend(StateBackend<K, W, ACC, AppendingState<IN, ACC>, InternalAppendingState<K, W, IN, ACC, ACC>>& state_backend) {
        this->m_state_backend = &state_backend;
    }

    std::shared_ptr<StreamOperator<OUT>> create_stream_operator(std::shared_ptr<StreamOperatorParameters<OUT>> parameters) override {
        // WindowAssigner
        WindowAssigner<IN, W>* window_assigner = m_window_assigner_creator();

        // WindowFunction
        std::shared_ptr<InternalWindowFunction<IN, OUT, K, W>> internal_window_function = m_internal_window_function_creator();
    
        // Trigger
        Trigger<IN, W>* trigger = m_trigger_creator();

        // StateDescriptor
        StateDescriptor<AppendingState<IN, ACC>, ACC>* state_descriptor = m_state_descriptor_creator();


        // KeyedStateBackend
        KeyGroupRange key_group_range = KeyGroupRangeAssignment::compute_key_group_range_for_operator_index(
                parameters->get_task_info()->get_number_of_parallel_subtasks(),
                parameters->get_task_info()->get_number_of_parallel_subtasks(),
                parameters->get_task_info()->get_index_of_subtask());
        // std::cout << "Key group range: " << key_group_range.to_string() << std::endl;

        AbstractKeyedStateBackendType* keyed_state_backend = m_state_backend->create_keyed_state_backend(
                parameters->get_execution_config(),
                parameters->get_task_info()->get_number_of_parallel_subtasks(),
                key_group_range);
        // AbstractKeyedStateBackendType* keyed_state_backend = m_state_backend->create_keyed_state_backend(
        //         parameters->get_execution_config(),
        //         3,
        //         KeyGroupRange(0, 10));

        // TODO: set current key
        keyed_state_backend->set_current_key(100);
        // TODO: register State Creator
        keyed_state_backend->register_state_creator(state_descriptor->get_state_descriptor_id(), m_state_creator);

        // ExecutionConfig
        ExecutionConfig& execution_config = parameters->get_execution_config();

        // ProcessingTimeService
        ProcessingTimeService& processing_time_service = parameters->get_processing_time_service();

        std::shared_ptr<WindowOperator<K, IN, ACC, OUT, W>> window_operator = std::make_shared<WindowOperator<K, IN, ACC, OUT, W>>(
                window_assigner,
                internal_window_function,
                trigger,
                state_descriptor,
                keyed_state_backend,
                execution_config,
                processing_time_service);

        window_operator->setup(parameters->get_output());

        return window_operator;
    }
};
