/**
 * Internal configuration for a StreamOperator.
 */
#pragma once
#include "../graph/StreamEdge.hpp"
#include "../../../core/config/Configuration.hpp"

#include <memory>
#include <vector>

class Configuration;

template <class OUT> class StreamEdge;

class StreamConfig
{
private:
    std::shared_ptr<Configuration>              m_config;
public:
    static std::string                          EDGE_NAME;
    static std::string                          NUMBER_OF_INPUTS;
    static std::string                          OPERATOR_FACTORY;

    StreamConfig(std::shared_ptr<Configuration> config): m_config(config) {}

    template <class OUT>
    std::shared_ptr<StreamEdge<OUT>>            get_out_edge();
    int                                         get_number_of_inputs();
    template <class IN, class OUT>
    std::shared_ptr<StreamOperatorFactory<OUT>> get_stream_operator_factory();
};



