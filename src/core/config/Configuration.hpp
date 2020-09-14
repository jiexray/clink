/**
 * Configuration object which stores key/value pairs.
 * This class can have two implementations, for task configuration and job configuration.
 */
#pragma once

#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "../../streaming/graph/StreamEdge.hpp"
#include "../../streaming/partitioner/ForwardPartitioner.hpp"
#include "../../streaming/operators/SimpleUdfStreamOperatorFactory.hpp"
#include "../../streaming/operators/StreamMap.hpp"
#include "../../streaming/operators/StreamSource.hpp"
#include "../../streaming/operators/StreamSink.hpp"
#include <cstring>


template <class T> class StreamEdge;
template <class IN, class OUT> class OneInputStreamOperator;
template <class OUT> class StreamOperatorFactory;

class Configuration
{
private:
    std::map<std::string, char*>                m_conf_data;

public:
    

    template <class T>
    void                                        set_value(std::string key, std::shared_ptr<T> value);
    template <class T>
    std::shared_ptr<T>                          get_value(std::string key);

    template <class T>
    void                                        set_edge(std::string key, std::shared_ptr<StreamEdge<T>> edge);
    template <class T>
    std::shared_ptr<StreamEdge<T>>              get_edge(std::string key);

    template <class IN, class OUT = std::string>
    void                                        set_operator_factory(std::string key, 
                                                                    std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory);
    template <class IN, class OUT = std::string>
    std::shared_ptr<StreamOperatorFactory<OUT>> get_operator_factory(std::string key);

    template <class IN, class OUT = std::string>                          
    void                                        set_stream_operator(std::string key,
                                                                        std::shared_ptr<StreamOperator<OUT>> stream_operator);
    template <class IN, class OUT = std::string>
    std::shared_ptr<StreamOperator<OUT>>        get_stream_operator(std::string key);
    
    // template <class IN, class OUT>                          
    // void                                        set_stream_operator(std::string key,
    //                                                                     std::shared_ptr<OneInputStreamOperator<IN, OUT>> stream_operator);
    // template <class IN, class OUT>
    // std::shared_ptr<OneInputStreamOperator<IN, OUT>>          
    //                                             get_stream_operator(std::string key);
};