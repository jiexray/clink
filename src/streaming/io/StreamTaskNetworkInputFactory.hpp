/**
 * A factory of creating StreamTaskNetworkInputFactory
 */
#pragma once
#include "StreamTaskNetworkInput.hpp"
#include "Tuple2.hpp"
#include "Tuple.hpp"
#include <typeinfo>

template<class T>
class StreamTaskNetworkInputFactory;

template <class T>
class StreamTaskNetworkInputFactory {
public:
    static std::shared_ptr<StreamTaskNetworkInput<T>> create_stream_task_network_input(std::shared_ptr<InputGate> input_gate) {
        return create_stream_task_network_input(input_gate, 0);
    }

    static std::shared_ptr<StreamTaskNetworkInput<T>> create_stream_task_network_input(std::shared_ptr<InputGate> input_gate,
                                                                                        int input_idx) {
        return std::make_shared<StreamTaskNetworkInput<T>>(input_gate, input_idx);
    }

    
};

template <template <class, class> class T, class T1, class T2>
class StreamTaskNetworkInputFactory<T<T1, T2>> {
public:
    static std::shared_ptr<StreamTaskNetworkInput<T<T1, T2>>> create_stream_task_network_input(std::shared_ptr<InputGate> input_gate, 
                                                                                            int input_index){
        const std::type_info& f0_type = typeid(T1);
        const std::type_info& f1_type = typeid(T2);

        std::shared_ptr<IOReadableWritable> tuple_deserialize_delegate = std::make_shared<TupleDeserializationDelegate>(2, 
                                                                            new std::reference_wrapper<const std::type_info>[2]{typeid(T1), typeid(T2)});
        
        return std::make_shared<StreamTaskNetworkInput<Tuple2<T1, T2>>>(input_gate, input_index, tuple_deserialize_delegate, RECORD_TYPE::TUPLE);
    } 

    static std::shared_ptr<StreamTaskNetworkInput<T<T1, T2>>> create_stream_task_network_input(std::shared_ptr<InputGate> input_gate) {
        return create_stream_task_network_input(input_gate, 0);
    }
};

