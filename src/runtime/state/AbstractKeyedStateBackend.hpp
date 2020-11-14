#pragma once
#include "KeyedStateBackend.hpp"
#include "InternalKvState.hpp"
#include "KeyGroupRange.hpp"
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
    typedef typename TemplateHelpUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelpUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelpUtil::ParamOptimize<SV>::type ParamSV;
    typedef typename TemplateHelpUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelpUtil::ParamOptimize<IS>::type ParamIS;
    typedef typename TemplateHelpUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelpUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelpUtil::ParamOptimize<SV>::const_type ConstParamSV;
    typedef typename TemplateHelpUtil::ParamOptimize<S>::const_type ConstParamS;
    typedef typename TemplateHelpUtil::ParamOptimize<IS>::const_type ConstParamIS;

    /** Listeners to change of m_key_context */
    std::vector<KeySelectionListener<K>*>   m_key_selection_listeners;

    std::unordered_map<std::string, InternalKvState<K, N, S>*> key_value_states_by_name;

    std::string m_last_name;

    InternalKvState<K, N, S>* m_last_state;

protected:
    /** The number of key-groups aka max parallelism. */
    int m_number_of_key_groups;

    KeyGroupRange m_key_group_range;

    
public:
};
