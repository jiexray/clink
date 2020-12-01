#pragma once
#include "WrappingFunction.hpp"
#include "InternalWindowFunction.hpp"
#include "AllWindowFunction.hpp"

/**
 * Internal window function for wrapping an AllWindowFunction that takes on Iterable
 * when the window state is a single value.
 */
template <class IN, class OUT, class W>
class InternalSingleValueAllWindowFunction:
    public WrappingFunction<AllWindowFunction<IN, OUT, W>>,
    public InternalWindowFunction<IN, OUT, char, W>
{
private:
    
public:
    InternalSingleValueAllWindowFunction(AllWindowFunction<IN, OUT, W>& wrapped_function): 
            WrappingFunction<AllWindowFunction<IN, OUT, W>>(wrapped_function) {}


    void process(
            char& key, 
            W& window, 
            InternalWindowFunctionContext& context, 
            IN* input, 
            std::shared_ptr<Collector<OUT>> out) {
        this->m_wrapped_function.apply(window, std::vector<IN*>{input}, out);
    }

    
};

