#pragma once
#include "InternalKeyContext.hpp"

/**
  The default InternalKeyContext implementation.
  
  @param <K>: Type of the key
 */
template <class K>
class InternalKeyContextImpl: public InternalKeyContext<K>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    KeyGroupRange   m_key_group_range;
    int             m_number_of_key_groups;
    K               m_current_key;
    int             m_current_key_group_index;
public:
    InternalKeyContextImpl(const KeyGroupRange& key_group_range, int number_of_key_groups):
    m_key_group_range(key_group_range),
    m_number_of_key_groups(number_of_key_groups) {}

    InternalKeyContextImpl(const InternalKeyContextImpl& other) {
        m_current_key_group_index = other.m_current_key_group_index;
        m_key_group_range = other.m_key_group_range;
    }

    ConstParamK get_current_key() {
        return m_current_key;
    }

    int get_current_key_group_index() {
        return m_current_key_group_index;
    }

    int get_number_of_key_groups() {
        return m_number_of_key_groups;
    }

    const KeyGroupRange& get_key_group_range() {
        return m_key_group_range;
    }

    void set_current_key(ConstParamK current_key) {
        m_current_key = current_key;
    }

    void set_current_key_group_index(int current_key_group_index) {
        m_current_key_group_index = current_key_group_index;
    }
};

