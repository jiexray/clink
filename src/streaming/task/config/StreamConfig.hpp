/**
 * Internal configuration for a StreamOperator.
 */
#pragma once
#include "StreamEdge.hpp"
#include "Configuration.hpp"

#include <memory>
#include <vector>


class StreamConfig
{
private:
    std::shared_ptr<Configuration>              m_config;
public:
    static std::string                          EDGE_NAME;
    static std::string                          NUMBER_OF_INPUTS;
    static std::string                          OPERATOR_FACTORY;
    static std::string                          OPERATOR_ID;
    static std::string                          OPERATOR_NAME;

    StreamConfig(std::shared_ptr<Configuration> config): m_config(config) {}

    template <class OUT>
    std::shared_ptr<StreamEdge<OUT>>            get_out_edge();
    int                                         get_number_of_inputs();
    template <class IN, class OUT, bool IS_SINK = false>
    std::shared_ptr<StreamOperatorFactory<OUT>> get_stream_operator_factory();

    std::string                                 get_operator_id();
    std::string                                 get_operator_name();
};

template <class OUT>
inline std::shared_ptr<StreamEdge<OUT>> StreamConfig::get_out_edge(){
    // return std::make_shared<StreamEdge<OUT>>();
    return m_config->get_edge<OUT>(StreamConfig::EDGE_NAME);
}

inline int StreamConfig::get_number_of_inputs() {
    return *(m_config->get_value<int>(StreamConfig::NUMBER_OF_INPUTS).get());
}

template <class IN, class OUT, bool IS_SINK>
inline std::shared_ptr<StreamOperatorFactory<OUT>> StreamConfig::get_stream_operator_factory() {
    return m_config->get_operator_factory<IN, OUT, IS_SINK>(StreamConfig::OPERATOR_FACTORY);
}

inline std::string StreamConfig::get_operator_id() {
    // TODO: implement get operator from config
    return "test-operator-id";
}

inline std::string StreamConfig::get_operator_name() {
    // TODO: implement get operator from config
    return "test-operator";
}
