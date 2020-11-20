#pragma once
#include "AbstractKeyedStateBackend.hpp"
#include "LoggerFactory.hpp"
#include "StateTable.hpp"
#include "NestedMapsStateTable.hpp"
#include <map>
#include <functional>
#include <typeinfo>

/**
  A AbstractKeyedStateBackend that keeps state on the Heap.

  @param <K> The key by which state is keyed.
  @param <N> the type of the namespace
  @param <SV> the type of the stored state value
  @param <S> the type of the public API state
  @param <IS> the type of internal state
 */
template <class K, class N, class SV, class S, class IS>
class HeapKeyedStatedBackend: public AbstractKeyedStateBackend<K, N, SV, S, IS>
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

    typedef typename std::function<IS*(const StateDescriptor<S, SV> &, StateTable<K, N, SV>&)> StateCreator;

    std::shared_ptr<spdlog::logger> m_logger = LoggerFactory::get_logger("HeapKeyedStatedBackend");

    std::map<std::string, StateCreator> STATE_FACTORIES;

    /** Map of registered Key/Value states. */
    std::map<std::string, StateTable<K, N, SV>*> m_registered_kv_states;

    StateTable<K, N, SV>* try_register_state_table(const StateDescriptor<S, SV>& state_desc) {
        StateTable<K, N, SV>* kv_state = nullptr;
        if (m_registered_kv_states.find(state_desc.get_name()) == m_registered_kv_states.end()) {
            kv_state = new NestedMapsStateTable<K, N, SV>(this->m_key_context);
            m_registered_kv_states[state_desc.get_name()] = kv_state;
        } else {
            SPDLOG_LOGGER_WARN(m_logger, "Register an existed StateTable: " + state_desc.get_name());
            kv_state = m_registered_kv_states[state_desc.get_name()];
        } 

        return kv_state;
    }

public:
    HeapKeyedStatedBackend(
            TaskKvStateRegistry<K, N, SV>* kv_state_registry, 
            ExecutionConfig* execution_config, 
            InternalKeyContext<K>* key_context,
            const std::map<std::string, StateTable<K, N, SV>*>& registered_kv_states):
            AbstractKeyedStateBackend<K, N, SV, S, IS>(kv_state_registry, execution_config, key_context),
            m_registered_kv_states(registered_kv_states) {}

    ~HeapKeyedStatedBackend() {
        // TODO: delete all kv_states in m_registered_kv_states
        // TODO: clear STATE_FACTORIES

    }

    bool register_state_creator(const std::string& type_id, StateCreator creator) {
        return STATE_FACTORIES.insert(std::make_pair(type_id, creator)).second;
    }


    IS* create_internal_state(const StateDescriptor<S, SV>& state_desc) override {
        std::string class_name = typeid(StateDescriptor<S, SV>).name();
        if (STATE_FACTORIES.find(class_name) == STATE_FACTORIES.end()) {
            throw std::runtime_error("Unregistered state class: " + class_name);
        }
        StateCreator state_creator = STATE_FACTORIES[class_name];

        StateTable<K, N, SV>* state_table = try_register_state_table(state_desc);

        return state_creator(state_desc, *state_table);
    }

};
