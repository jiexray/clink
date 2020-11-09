/**
 * A factory of creating StreamTaskNetworkInputFactory
 */
#pragma once
#include "StreamTaskNetworkInput.hpp"
// #include "TupleDeserializationDelegateV2.hpp"
// #include "Tuple2.hpp"
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