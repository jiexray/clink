#include "StreamConfig.hpp"

std::string StreamConfig::EDGE_NAME             = "task_out_edge";
std::string StreamConfig::NUMBER_OF_INPUTS      = "number_of_inputs";
std::string StreamConfig::OPERATOR_FACTORY      = "operator_factory";

template <class OUT>
std::shared_ptr<StreamEdge<OUT>> StreamConfig::get_out_edge(){
    // return std::make_shared<StreamEdge<OUT>>();
    return m_config->get_edge<OUT>(StreamConfig::EDGE_NAME);
}

int StreamConfig::get_number_of_inputs() {
    return *(m_config->get_value<int>(StreamConfig::NUMBER_OF_INPUTS).get());
}

template std::shared_ptr<StreamEdge<std::string>> StreamConfig::get_out_edge();


template <class IN, class OUT>
std::shared_ptr<StreamOperatorFactory<OUT>> StreamConfig::get_stream_operator_factory() {
    return m_config->get_operator_factory<IN, OUT>(StreamConfig::OPERATOR_FACTORY);
}

template std::shared_ptr<StreamOperatorFactory<std::string>> StreamConfig::get_stream_operator_factory<std::string, std::string>();