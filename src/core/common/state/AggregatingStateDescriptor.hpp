#pragma once

#include "AggregateFunction.hpp"
#include "StateDescriptor.hpp"
#include "AggregatingState.hpp"
#include "TemplateHelper.hpp"

/**
  A StateDescriptor for AggregatingState.

  @param IN The type of the values that are added to the state.
  @param ACC The type of the accumulator.
  @param OUT The type of the values that are returned from the state.
 */
template <class IN, class ACC, class OUT>
class AggregatingStateDescriptor: 
        public StateDescriptor<AggregatingState<IN, OUT>, ACC>,
        public StateDescriptor<AppendingState<IN, OUT>, ACC> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::type ParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::const_type ConstParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::type ParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::const_type ConstParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::type ParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::const_type ConstParamOUT;

    AggregateFunction<IN, ACC, OUT>& m_agg_function;

public:
    AggregatingStateDescriptor(
            const std::string& name, 
            AggregateFunction<IN, ACC, OUT>& agg_function,
            ConstParamACC default_value):
            StateDescriptor<AggregatingState<IN, OUT>, ACC>(name, default_value),
            StateDescriptor<AppendingState<IN, OUT>, ACC>(name, default_value),
            m_agg_function(agg_function) {}

    StateDescriptorType get_type() override {
        return StateDescriptorType::AGGREGATING;
    }

    AggregateFunction<IN, ACC, OUT>& get_aggregate_function() const {
        return m_agg_function;
    }

    /* Properties */
    std::string get_name() const{
        return StateDescriptor<AggregatingState<IN, OUT>, ACC>::get_name();
    }

    ConstParamACC get_default_value() const {
        return StateDescriptor<AggregatingState<IN, OUT>, ACC>::get_default_value();
    }
};