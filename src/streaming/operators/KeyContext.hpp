#pragma once
#include "TemplateHelper.hpp"

/**
  Interface for setting and querying the current key of keyed operations.
 */
template <class K>
class KeyContext {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
public:
    virtual void set_current_key(ConstParamK key) = 0;

    virtual ConstParamK get_current_key() = 0;
};


template <class K>
class TestKeyContext: public KeyContext<K> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;

    K m_key;
public:
    TestKeyContext(ConstParamK key): m_key(key) {}

    void set_current_key(ConstParamK key) {
        m_key = key;
    }

    ConstParamK get_current_key() {
        return m_key;
    } 
};