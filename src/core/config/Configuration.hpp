/**
 * Configuration object which stores key/value pairs.
 * This class can have two implementations, for task configuration and job configuration.
 */
#pragma once

#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "StreamEdge.hpp"
#include "ForwardPartitioner.hpp"
#include "SimpleUdfStreamOperatorFactory.hpp"
#include "StreamMap.hpp"
#include "StreamSource.hpp"
#include "StreamSink.hpp"
#include "StreamFlatMap.hpp"
#include <cstring>


template <class T> class StreamEdge;
template <class IN, class OUT> class OneInputStreamOperator;
template <class OUT> class StreamOperatorFactory;

template <class IN, class OUT> class StreamFlatMap;
template <class T, class O> class FlatMapFunction;

template <class IN, class OUT> class StreamMap;
template <class T, class O> class MapFunction;



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


//------------------------------------------------------
// string type configuration
//------------------------------------------------------

template<>
inline void Configuration::set_value<std::string>(std::string key, std::shared_ptr<std::string> value) {
    // TODO
    throw std::runtime_error("set string value is not implemented yet");
}

template<>
inline std::shared_ptr<std::string> Configuration::get_value<std::string>(std::string key) {
    // TODO
    throw std::runtime_error("get string value is not implement yes");
    // if (m_conf_data.find(key) != m_conf_data.end()) {
    //     return std::make_shared<std::string>()
    // }
}

//------------------------------------------------------
// int type configuration
//------------------------------------------------------


template<>
inline void Configuration::set_value<int>(std::string key, std::shared_ptr<int> value) {
    if (value == nullptr) 
        return;

    if (this->m_conf_data.find(key) != this->m_conf_data.end()) {
        throw std::runtime_error("key " + key + " has already inserted into the conf_data");
    }
    char* value_in_char = new char[sizeof(int) + 1];
    memcpy(value_in_char, value.get(), sizeof(int));
    m_conf_data.insert(std::make_pair(key, value_in_char));
}

template<>
inline std::shared_ptr<int> Configuration::get_value<int>(std::string key) {
    if (m_conf_data.find(key) == m_conf_data.end()) {
        return nullptr;
    }

    char                    *value_in_char      = m_conf_data[key];
    std::shared_ptr<int>    value               = std::make_shared<int>(*((int*)value_in_char));

    // remove key
    free(value_in_char);
    m_conf_data.erase(key);

    return value; 
}


//------------------------------------------------------
// edge configuration
//------------------------------------------------------

template<class T>
inline void Configuration::set_edge(std::string key, std::shared_ptr<StreamEdge<T>> edge){
    if (edge == nullptr) 
        return;

    if (this->m_conf_data.find(key) != this->m_conf_data.end()) {
        throw std::runtime_error("key " + key + " has already inserted into the conf_data");
    }

    // insert edge
    char *edge_in_char = new char[sizeof(StreamEdge<T>) + 1];
    memcpy(edge_in_char, edge.get(), sizeof(StreamEdge<T>));
    this->m_conf_data.insert(std::make_pair(key, edge_in_char));

    // insert edge partition
    std::shared_ptr<StreamPartitioner<T>> edge_partitioner = edge->get_output_partitioner();
    if (std::dynamic_pointer_cast<ForwardPartitioner<T>>(edge_partitioner) != nullptr) {
        // std::cout << "edge: " << edge->get_edge_id() << " use ForwardPartition" << std::endl;
        char* edge_partitioner_in_char = new char[sizeof(ForwardPartitioner<T>) + 1];
        memcpy(edge_partitioner_in_char, 
                std::dynamic_pointer_cast<ForwardPartitioner<T>>(edge_partitioner).get(), 
                sizeof(ForwardPartitioner<T>));
        m_conf_data.insert(std::make_pair(key + "_partitioner", edge_partitioner_in_char));
    } else {
        throw std::runtime_error("unknown partitioner type");
    }
}

template<class T>
inline std::shared_ptr<StreamEdge<T>> Configuration::get_edge(std::string key){
    if (m_conf_data.find(key) != m_conf_data.end()) {
        char                    *edge_in_char               = m_conf_data[key];
        StreamEdge<T>           *edge_ptr                   = (StreamEdge<T>*)edge_in_char;

        char                    *edge_partitioner_in_char   = m_conf_data[key + "_partitioner"];
        StreamPartitioner<T>    *edge_partitioner_ptr       = (StreamPartitioner<T>*)edge_partitioner_in_char;
        std::string             partition_type_str          = edge_partitioner_ptr->to_string();

        std::shared_ptr<StreamPartitioner<T>> edge_partitioner;

        if (partition_type_str == "FORWARD") {
            // std::cout << "get_edge() edge " << edge_ptr->get_edge_id() << " use partitioner FORWARD" << std::endl;
            edge_partitioner = std::make_shared<ForwardPartitioner<T>>();
            edge_partitioner->setup(edge_partitioner_ptr->get_number_of_channels());
        }

        // reconstruct the StreamEdge
        std::shared_ptr<StreamEdge<T>> edge = std::make_shared<StreamEdge<T>>(edge_ptr->get_source_id(), 
                                                                            edge_ptr->get_target_id(), 
                                                                            edge_ptr->get_source_operator_name(),
                                                                            edge_ptr->get_target_operator_name(), edge_partitioner);
        
        // free the buffer store the StreamEdge
        free(edge_in_char);
        m_conf_data.erase(key);
        return edge;
    } else {
        return nullptr;
    }
}

//------------------------------------------------------
// Stream operator factory configuration
//------------------------------------------------------
template <class IN, class OUT>
inline void Configuration::set_operator_factory(std::string key, 
                                        std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory){
    if (this->m_conf_data.find(key) != this->m_conf_data.end()) {
        throw std::runtime_error("key " + key + " has already inserted into the conf_data");
    }
    // check the type of StreamOperatorFactory
    if (std::dynamic_pointer_cast<SimpleUdfStreamOperatorFactory<OUT>>(operator_factory) != nullptr) {
        // (1) serialize the SimpleUdfStreamOperatorFactory
        std::shared_ptr<SimpleUdfStreamOperatorFactory<OUT>> udf_operator_factory = std::dynamic_pointer_cast<SimpleUdfStreamOperatorFactory<OUT>>(operator_factory);
        char* udf_operator_factory_in_char = new char[sizeof(SimpleUdfStreamOperatorFactory<OUT>) + 1];
        memcpy(udf_operator_factory_in_char, udf_operator_factory.get(), sizeof(SimpleUdfStreamOperatorFactory<OUT>));
        this->m_conf_data.insert(std::make_pair(key, udf_operator_factory_in_char));

        // serialize the operator
        std::shared_ptr<StreamOperator<OUT>> stream_operator = udf_operator_factory->get_operator();
        set_stream_operator<IN, OUT>(key + "_operator", stream_operator);

    } else {
        throw std::runtime_error("Not support other stream operator factory except for SimpleUdfStreamOperatorFactory.");
    }
}

template <class IN, class OUT>
inline std::shared_ptr<StreamOperatorFactory<OUT>> Configuration::get_operator_factory(std::string key) {
    if (m_conf_data.find(key) != m_conf_data.end()) {
        std::shared_ptr<StreamOperator<OUT>> stream_operator = get_stream_operator<IN, OUT>(key + "_operator");

        if (stream_operator == nullptr) {
            throw std::runtime_error("Error when deserialize operator factory, stream operator is null");
        }

        StreamOperatorFactory<OUT> *operator_factory = (StreamOperatorFactory<OUT> *)m_conf_data[key];
        // std::cout << "[DEBUG] Configuration::get_operator_factory(): after get operator" << std::endl;

        if (dynamic_cast<SimpleStreamOperatorFactory<OUT>*>(operator_factory) != nullptr) {
            // std::cout << "[DEBUG] Configuration::get_operator_factory(): is SimpleStreamOperatorFactory" << std::endl;
            std::shared_ptr<StreamOperatorFactory<OUT>> gen_factory = SimpleStreamOperatorFactory<OUT>::of(stream_operator);
            // std::cout << "[DEBUG] Configuration::get_operator_factory(): after create stream operator" << std::endl;
            return gen_factory;
        } else {
            throw std::runtime_error("Stream operator factory deserializtion not support other stream operator factory except for SimpleStreamOperatorFactory.");
        }
    } else {
        return nullptr;
    }
}

template <class IN, class OUT>                          
inline void Configuration::set_stream_operator(std::string key, std::shared_ptr<StreamOperator<OUT>> stream_operator) {
    if (this->m_conf_data.find(key) != this->m_conf_data.end()) {
        throw std::runtime_error("key " + key + " has already inserted into the conf_data");
    }
    // check the type of opertor
    if (std::dynamic_pointer_cast<StreamMap<IN, OUT>>(stream_operator) != nullptr) {
        std::shared_ptr<StreamMap<IN, OUT>> map_operator = std::dynamic_pointer_cast<StreamMap<IN, OUT>>(stream_operator);
        // char* map_operator_in_char = new char[sizeof(StreamMap<IN, OUT>) + 1];
        // memcpy(map_operator_in_char, map_operator.get(), sizeof(StreamMap<IN, OUT>));
        // this->m_conf_data.insert(std::make_pair(key, map_operator_in_char));

        // serialier the user function in it
        std::shared_ptr<MapFunction<IN, OUT>> map_function = map_operator->get_user_function();
        char* map_function_in_char = map_function->serialize();
        this->m_conf_data.insert(std::make_pair(key + "_user-function", map_function_in_char));

    } else if (std::dynamic_pointer_cast<StreamSource<OUT>>(stream_operator) != nullptr) {
        // std::cout << "[DEBUG] serialize StreamSource operator" << std::endl;
        std::shared_ptr<StreamSource<OUT>> source_operator = std::dynamic_pointer_cast<StreamSource<OUT>>(stream_operator);

        std::shared_ptr<SourceFunction<OUT>> source_function = source_operator->get_user_function();
        char* source_function_in_char = source_function->serialize();
        this->m_conf_data.insert(std::make_pair(key + "_user-function", source_function_in_char));

    } else if (std::dynamic_pointer_cast<StreamSink<IN>>(stream_operator) != nullptr){
        // std::cout << "[DEBUG] serialize StreamSource operator" << std::endl;
        std::shared_ptr<StreamSink<IN>> sink_operator = std::dynamic_pointer_cast<StreamSink<IN>>(stream_operator);

        std::shared_ptr<SinkFunction<IN>> sink_function = sink_operator->get_user_function();
        char* sink_function_in_char = sink_function->serialize();
        this->m_conf_data.insert(std::make_pair(key + "_user-function", sink_function_in_char));
    } else if (std::dynamic_pointer_cast<StreamFlatMap<IN, OUT>>(stream_operator) != nullptr) {
        std::shared_ptr<StreamFlatMap<IN, OUT>> flat_map_operator = std::dynamic_pointer_cast<StreamFlatMap<IN, OUT>>(stream_operator);

        std::shared_ptr<FlatMapFunction<IN, OUT>> flat_map_function = flat_map_operator->get_user_function();
        char* flat_map_function_in_char = flat_map_function->serialize();
        this->m_conf_data.insert(std::make_pair(key + "_user-function", flat_map_function_in_char));
    } else {
        throw std::runtime_error("Not support other stream operator except for StreamMap, StreamSource, StreamSink, StreamFlatMap");
    }
}


template <class IN, class OUT>
inline std::shared_ptr<StreamOperator<OUT>>  Configuration::get_stream_operator(std::string key){
    std::string func_key = key + "_user-function";
    if (m_conf_data.find(func_key) != m_conf_data.end()) {
        Function* func_ptr = (Function*) (m_conf_data[func_key]);
        if (dynamic_cast<MapFunction<IN, OUT>*>(func_ptr) != nullptr) {
            std::shared_ptr<MapFunction<IN, OUT>> gen_func = (dynamic_cast<MapFunction<IN, OUT>*>(func_ptr))->deserialize();
            return std::make_shared<StreamMap<IN, OUT>>(gen_func);
        } else if(dynamic_cast<SourceFunction<OUT>*>(func_ptr) != nullptr) {
            std::shared_ptr<SourceFunction<OUT>> gen_func = (dynamic_cast<SourceFunction<OUT>*>(func_ptr))->deserialize();
            return std::make_shared<StreamSource<OUT>>(gen_func);
        } else if(dynamic_cast<SinkFunction<IN>*>(func_ptr) != nullptr) {
            std::shared_ptr<SinkFunction<IN>> gen_func = (dynamic_cast<SinkFunction<IN>*>(func_ptr))->deserialize(m_conf_data[func_key]);
            return std::make_shared<StreamSink<IN, OUT>>(gen_func);            
        } else if(dynamic_cast<FlatMapFunction<IN, OUT>*>(func_ptr) != nullptr) {
            std::shared_ptr<FlatMapFunction<IN, OUT>> gen_func = (dynamic_cast<FlatMapFunction<IN, OUT>*>(func_ptr))->deserialize();
            return std::make_shared<StreamFlatMap<IN, OUT>>(gen_func);
        } else {
            throw std::runtime_error("Stream operator deserialization only support StreamMap, StreamSource, StreamSink, StreamFlatMap at function: " + std::string(__PRETTY_FUNCTION__));
        }
    } else {
        return nullptr;
    }
}