#pragma once

#include "AbstractHeapState.hpp"
#include "InternalAggregatingState.hpp"
#include "StateTransformationFunction.hpp"
#include "AggregatingStateDescriptor.hpp"
#include "AggregateFunction.hpp"

template <class IN, class ACC, class OUT>
class AggregateTransformation: public StateTransformationFunction<ACC, IN> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::const_type ConstParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::const_type ConstParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::const_type ConstParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::ret_ptr_type RetACC;

    AggregateFunction<IN, ACC, OUT>& m_agg_function;
public:
    AggregateTransformation(AggregateFunction<IN, ACC, OUT>& agg_function): m_agg_function(agg_function){}

    ACC* apply(ACC* accumulator, ConstParamIN value) override {
        if (accumulator == nullptr) {
            ACC* tmp_accumulator = m_agg_function.create_accumulator();
            if (tmp_accumulator == nullptr) {
                throw std::runtime_error("AggregateFunction cannot initialize a empty accumulator");
            }
            ACC* res = m_agg_function.add(&value, tmp_accumulator);
            delete tmp_accumulator;
            return res;
        } else {
            return m_agg_function.add(&value, accumulator);
        }
    }

    /* Properties */
    AggregateFunction<IN, ACC, OUT>& get_agg_function() {
        return m_agg_function;
    }
};

/**
  Heap-backed partitioned AggregatingState that is snapshotted into files.
 */
template <class K, class N, class IN, class ACC, class OUT>
class HeapAggregatingState:
        public AbstractHeapState<K, N, ACC>,
        public InternalAggregatingState<K, N, IN, ACC, OUT> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::type ParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<IN>::const_type ConstParamIN;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::type ParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::const_type ConstParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::type ParamOUT;
    typedef typename TemplateHelperUtil::ParamOptimize<OUT>::const_type ConstParamOUT;

    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;

    AggregateTransformation<IN, ACC, OUT>* m_aggregate_transformation;
public:
    HeapAggregatingState(
            StateTable<K, N, ACC>& state_table,
            ConstParamACC default_value,
            AggregateFunction<IN, ACC, OUT>& aggregate_function):
            AbstractHeapState<K, N, ACC>(state_table, default_value) {
        m_aggregate_transformation = new AggregateTransformation<IN, ACC, OUT>(aggregate_function);
    }
    ~HeapAggregatingState() {
        delete m_aggregate_transformation;
    }

    // ---------------------------------------------------------------
    //  state access
    // ---------------------------------------------------------------

    ConstParamACC get_internal() override {
        return this->m_state_table.get(this->m_current_namespace);
    }

    void update_internal(ParamACC value_to_store) override {
        this->m_state_table.put(this->m_current_namespace, value_to_store);
    }

    void add_internal(ConstParamIN value) override {
        // throw std::runtime_error("")
        this->add(value);
    }

    bool contains_internal() override {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            return false;
        }
        return true;
    }

    /**
      The return value need to be freed, if the value type is not fundamental types.

      TODO: change the return value type of get()
     */
    ConstParamOUT get() override {
        ConstParamACC accumulator = get_internal();
        OUT* res = m_aggregate_transformation->get_agg_function().get_result(&accumulator);

        if (std::is_fundamental<OUT>::value) {
            OUT res_val = *res;
            delete res;
            return res_val;
        } else {
            return *res;
        }
        // return *m_aggregate_transformation->get_agg_function().get_result(&accumulator);
    }

    void add(ConstParamIN value) override {
        if (&value == nullptr) {
            this->clear();
            return;
        }

        this->m_state_table.template transform<IN>(this->m_current_namespace, value, *m_aggregate_transformation);
    }

    template<class IS>
    static IS* create(const StateDescriptor<AggregatingState<IN, OUT>, ACC>& state_desc, StateTable<K, N, ACC>& state_table) {
        TemplateHelperUtil::CheckInherit<StateDescriptor<AggregatingState<IN, OUT>, ACC>, AggregatingStateDescriptor<IN, OUT, ACC>>::assert_inherit();
        return (IS*)new HeapAggregatingState(state_table, state_desc.get_default_value(), (dynamic_cast<const AggregatingStateDescriptor<IN, OUT, ACC>*>(&state_desc))->get_aggregate_function());
    }

    template<class IS>
    static IS* create_appending(const StateDescriptor<AppendingState<IN, OUT>, ACC>& state_desc, StateTable<K, N, ACC>& state_table) {
        TemplateHelperUtil::CheckInherit<StateDescriptor<AggregatingState<IN, OUT>, ACC>, AggregatingStateDescriptor<IN, OUT, ACC>>::assert_inherit();
        return (IS*)new HeapAggregatingState(state_table, state_desc.get_default_value(), (dynamic_cast<const AggregatingStateDescriptor<IN, OUT, ACC>*>(&state_desc))->get_aggregate_function());
    }
};

