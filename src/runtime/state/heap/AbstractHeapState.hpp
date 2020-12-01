#pragma once
#include "InternalKvState.hpp"
#include "StateTable.hpp"

/**
  Base class for partitioned State implementations that are backed by a regular
  heap hash map. 

  @param <K> The type of the key.
  @param <N> The type of the namespace.
  @param <SV> The type of the values in the state.
 */
template <class K, class N, class SV>
class AbstractHeapState : virtual public InternalKvState<K, N, SV>{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::const_type ConstParamSV;

    SV                          m_default_value;

protected:
    /** Map containing the actual key/value pairs */
    StateTable<K, N, SV>&       m_state_table;

    /** The current namespace, which the access methods will refer to. */
    // TODO: copy construct namespace, maybe it can be optimized.
    N                           m_current_namespace;

public:
    AbstractHeapState(
        StateTable<K, N, SV>& state_table,
        ConstParamSV default_value):
        m_state_table(state_table),
        m_default_value(default_value){}

    void clear() override {
        m_state_table.remove(m_current_namespace);
    }

    void set_current_namespace(ConstParamN ns) override {
        m_current_namespace = ns;
    }
};

