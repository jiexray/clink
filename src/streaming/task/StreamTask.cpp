#include "StreamTask.hpp"

/**
 * Constructor of StreamTask, initialize result writer.
 */
template <class OUT>
StreamTask<OUT>::StreamTask(std::shared_ptr<Environment> env): AbstractInvokable(env) {
    m_configuration = std::make_shared<StreamConfig>(env->get_task_configuration());
    std::shared_ptr<StreamEdge<OUT>> edge = m_configuration->get_out_edge<OUT>();

    if (edge != nullptr) {
        this->m_result_writer = create_result_writer(edge, 0, env->get_task_info()->get_task_name());
    } else {
        std::cout << "[INFO] Task " << env->get_task_info()->get_task_name() << " do not have out edges" << std::endl;
    }

}

/**
 * Create a ResultWriter for the StreamTask.
 * @parameter edge: the edge (logical edge) for this physical subtask.
 * @parameter output_idx: the output index for this stream task in the TaskManager, which 
 *   takes charge of creating ResultPartitions. Currently, the output_idx can only be zero,
 *   for a StreamTask (JobVertex) only have one out edge (represent a StreamEdge between consequtive
 *   tasks).
 * @paramenter task_name: the name of the subtask.
 */
template <class OUT>
std::shared_ptr<ResultWriter<OUT>> StreamTask<OUT>::create_result_writer(std::shared_ptr<StreamEdge<OUT>> edge, int output_idx, std::string task_name) {
    std::shared_ptr<StreamPartitioner<OUT>> output_partitioner = edge->get_output_partitioner();

    // TODO: add logging module
    std::cout << "Using partitioner " << output_partitioner->to_string() << " for output " << output_idx << " of task " << task_name << std::endl;

    std::shared_ptr<ResultPartition> buffer_writter = this->get_environment()->get_writer(output_idx);
    return std::make_shared<ChannelSelectorResultWriter<OUT>>(buffer_writter, task_name, output_partitioner);
}

template <class OUT>
void StreamTask<OUT>::process_input() {
    InputStatus status = m_input_processor->process_input();
    // TODO: process the return status of input_procesor
}

template<class OUT>
void StreamTask<OUT>::before_invoke() {
    init();

    // request partitions
    request_partitions();
}

template<class OUT>
void StreamTask<OUT>::request_partitions() {
    std::shared_ptr<InputGate> input_gate = get_environment()->get_input_gate(0);
    if (input_gate != nullptr) {
        input_gate->request_partitions();
        std::cout << "[INFO] StreamTask " << get_name() << " finish requesting partitions" << std::endl;
    }
}

template<class OUT>
void StreamTask<OUT>::invoke() {
    before_invoke();

    // TODO: start processing record
    std::cout << "[INFO] StreamTask " << get_name() << " start to process record" << std::endl;
}

template class StreamTask<std::string>;
