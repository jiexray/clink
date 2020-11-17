#pragma once
#include "KeyedStateBackend.hpp"
#include "InternalKvState.hpp"
#include "KeyGroupRange.hpp"
#include "TaskKvStateRegistry.hpp"
#include "ExecutionConfig.hpp"
#include "InternalKeyContext.hpp"
#include "KeyGroupRangeAssignment.hpp"
#include <vector>
#include <map>
#include <unordered_map>

/**
  Base implementation of KeyStateBackend.

  @param <K> The key by which state is keyed.
  @param <N> the type of the namespace
  @param <SV> the type of the stored state value
  @param <S> the type of the public API state
  @param <IS> the type of internal state
 */
template <class K, class N, class SV, class S, class IS>
class AbstractKeyedStateBackend: public KeyedStateBackend<K, N, SV, S, IS>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<IS>::type ParamIS;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<SV>::const_type ConstParamSV;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<IS>::const_type ConstParamIS;

    /** Listeners to change of m_key_context */
    std::vector<KeySelectionListener<K>*>   m_key_selection_listeners;

    std::unordered_map<std::string, InternalKvState<K, N, SV>*> m_key_value_states_by_name;

    std::string m_last_name;

    InternalKvState<K, N, SV>* m_last_state;

protected:
    /** The number of key-groups aka max parallelism. */
    int m_number_of_key_groups;

    /** Range of key-groups for which this backend is responsible */
    KeyGroupRange m_key_group_range;

    /** KvStateRegistry helper for this task*/
    TaskKvStateRegistry<K, N, SV>* m_kv_state_registry;

    ExecutionConfig* m_execution_config;

    InternalKeyContext<K>* m_key_context;

    void notify_key_selected(ConstParamK new_key) {
        for (int i = 0; i < m_key_selection_listeners.size(); i++) {
            this->m_key_selection_listeners[i]->key_selected(new_key);
        }
    }
public:
    AbstractKeyedStateBackend(
            TaskKvStateRegistry<K, N, SV>* kv_state_registry, 
            ExecutionConfig* execution_config, 
            InternalKeyContext<K>* key_context):
            m_kv_state_registry(kv_state_registry),
            m_execution_config(execution_config),
            m_key_context(key_context){
        m_number_of_key_groups = key_context->get_number_of_key_groups();

        TemplateHelperUtil::CheckInherit<State, S>::assert_inherit();
        TemplateHelperUtil::CheckInherit<InternalKvState<K, N, SV>, IS>::assert_inherit();
    }

    void set_current_key(ConstParamK new_key) override{
        notify_key_selected(new_key);
        this->m_key_context->set_current_key(new_key);
        this->m_key_context->set_current_key_group_index(KeyGroupRangeAssignment::assign_to_key_group<K>(new_key, m_number_of_key_groups));
    }

    ConstParamK get_current_key() override {
        return this->m_key_context->get_current_key();
    }

    void register_key_selection_listener(KeySelectionListener<K>* listener) override {
        this->m_key_selection_listeners.push_back(listener);
    }

    bool deregister_key_selection_listener(KeySelectionListener<K>* listener) override {    
        for (int i = 0; i < this->m_key_selection_listeners.size(); i++) {
            if (this->m_key_selection_listeners[i] == listener) {
                this->m_key_selection_listeners.erase(this->m_key_selection_listeners.begin() + i);
                return true;
            }
        }
        return false;
    }

    ParamS get_or_create_keyed_state(const StateDescriptor<S, SV>& state_descriptor) override {
        InternalKvState<K, N, SV>* kv_state = nullptr;
        if (this->m_key_value_states_by_name.find(state_descriptor.get_name()) == this->m_key_value_states_by_name.end()) {
            // TODO: create KvState with TtlStateFactory
            kv_state = this->create_internal_state(state_descriptor);
            m_key_value_states_by_name[state_descriptor.get_name()] = kv_state;
        } else {
            kv_state = m_key_value_states_by_name[state_descriptor.get_name()];
        }
        
        return *((S*)kv_state);
    }

    ParamIS get_or_create_internal_keyed_state(const StateDescriptor<S, SV>& state_descriptor) override {
        InternalKvState<K, N, SV>* kv_state = nullptr;
        if (this->m_key_value_states_by_name.find(state_descriptor.get_name()) == this->m_key_value_states_by_name.end()) {
            // TODO: create KvState with TtlStateFactory
            kv_state = this->create_internal_state(state_descriptor);
            m_key_value_states_by_name[state_descriptor.get_name()] = kv_state;
        } else {
            kv_state = m_key_value_states_by_name[state_descriptor.get_name()];
        }
        
        return *(dynamic_cast<IS*>(kv_state));
    }

    ParamS get_partitioned_state(ConstParamN ns, const StateDescriptor<S, SV>& state_descriptor) {
        if (state_descriptor.get_name() == m_last_name) {
            m_last_state->set_current_namespace(ns);
            return *((S*)m_last_state);
        }

        if (m_key_value_states_by_name.find(state_descriptor.get_name()) != m_key_value_states_by_name.end()) {
            InternalKvState<K, N, SV>* previous = m_key_value_states_by_name[state_descriptor.get_name()];
            m_last_state = previous;
            m_last_state->set_current_namespace(ns);
            m_last_name = state_descriptor.get_name();
            return *((S*)previous);
        }
    
        ParamS state = get_or_create_keyed_state(state_descriptor);
        InternalKvState<K, N, SV>* kv_state = (InternalKvState<K, N, SV>*)(&state);

        m_last_name = state_descriptor.get_name();
        m_last_state = kv_state;
        m_last_state->set_current_namespace(ns);

        return state;
    }
};
