#pragma once

#include "AbstractHeapState.hpp"
#include "InternalAppendingState.hpp"


/**
  Base class for AppendindState (InternalAppendingState) that is stored on the heap.
 */
template<class K, class N, class IN, class SV, class OUT>
class AbstractHeapAppendingState: public virtual AbstractHeapState<K, N, SV>, public virtual InternalAppendingState<K, N, IN, SV, OUT> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::const_type ConstParamSV;    
public:
    AbstractHeapAppendingState(
        StateTable<K, N, SV>& state_table,
        ConstParamSV default_value): 
        AbstractHeapState<K, N, SV>(state_table, default_value) {

    }

    ParamSV get_internal() override {
        return this->m_state_table.get(this->m_current_namespace);
    }

    void update_internal(ParamSV value_to_store) override {
        this->m_state_table.put(this->m_current_namespace, value_to_store);
    }
};

