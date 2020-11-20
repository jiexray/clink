#pragma once
#include <vector>
#include "InternalListState.hpp"
#include "AbstractHeapAppendingState.hpp"

/**
  Heap-backed partitioned ListState.
 */
template <class K, class N, class V>
class HeapListState:
    public InternalListState<K, N, V>,
    public AbstractHeapState<K, N, std::vector<V>> {
private:
    typedef typename std::vector<V> SV;
    typedef typename TemplateHelperUtil::ParamOptimize<V>::type ParamV;
    typedef typename TemplateHelperUtil::ParamOptimize<V>::const_type ConstParamV;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::const_type ConstParamSV;
public:
    HeapListState(
        StateTable<K, N, std::vector<V>>& state_table,
        const std::vector<V>& default_value):
        AbstractHeapState<K, N, std::vector<V>>(state_table, default_value) {}

    // ---------------------------------------------------------------
    //  state access
    // ---------------------------------------------------------------

    ParamSV get_internal() override {
        return this->m_state_table.get(this->m_current_namespace);
    }

    void update_internal(ParamSV value_to_store) override {
        this->m_state_table.put(this->m_current_namespace, value_to_store);
    }

    std::vector<V>& get() override {
        return this->get_internal();
    }

    void add(ConstParamV value) {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            this->m_state_table.put(this->m_current_namespace, std::vector<V>());
        }
        std::vector<V>& list = this->m_state_table.get(this->m_current_namespace);
        list.push_back(value);
    }

    void update(const std::vector<V>& values) override {
        if (values.empty()) {
            this->clear();
            return;
        }

        this->m_state_table.put(this->m_current_namespace, values);
    }

    void add_all(const std::vector<V>& values) override {
        if (!values.empty()) {
            if (!this->m_state_table.contains_key(this->m_current_namespace)) {
                this->m_state_table.put(this->m_current_namespace, std::vector<V>());
            }
            std::vector<V>& previous_state = this->m_state_table.get(this->m_current_namespace);

            for (int i = 0; i < values.size(); i++) {
                previous_state.push_back(values[i]);
            }
        }
    }

    template<class IS>
    static IS* create(const StateDescriptor<ListState<V>, std::vector<V>>& state_desc, StateTable<K, N, std::vector<V>>& state_table) {
        return (IS*)new HeapListState(state_table, state_desc.get_default_value());
    }
};
