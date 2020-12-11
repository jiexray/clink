#pragma once
#include "KeyedMapStateStore.hpp"
#include "KeyedListStateStore.hpp"
#include <memory>
#include "Collector.hpp"
#include "TemplateHelper.hpp"
#include "Function.hpp"

/**
  A context for InternalWindowFunction
 */
class InternalWindowFunctionContext {
public:
    virtual long current_process_time() = 0;

    virtual long current_watermark() = 0;

    virtual KeyedMapStateStore& window_map_state() = 0;

    virtual KeyedListStateStore& window_list_state() = 0;

    virtual KeyedMapStateStore& global_map_state() = 0;

    virtual KeyedListStateStore& global_list_state() = 0;
};

/**
  Internal interface for functions that are evaluated over keyed (grouped) windows.

  @param <IN> The type of the input value.
  @param <OUT> The type of the output value.
  @param <KEY> The type of the key.
 */
template <class IN, class OUT, class K, class W>
class InternalWindowFunction: public Function {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
public:
    virtual void process(
            ConstParamK key, 
            const W& window, 
            InternalWindowFunctionContext& context, 
            const IN& input, 
            std::shared_ptr<Collector<OUT>> out) = 0;
};

