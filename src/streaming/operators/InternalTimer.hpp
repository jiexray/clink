#pragma once
#include "TemplateHelper.hpp"
#include <string>

/**
  Internal interface for in-flight timers.

  @param <K> Type of the keys to which timers are scoped.
  @param <N> Type of the namespace to which timers are scoped.
 */
template <class K, class N>
class InternalTimer
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;

public:
    virtual long get_timestamp() const = 0;

    virtual ConstParamK get_key() const = 0;

    virtual ConstParamN get_namespace() const = 0;

    virtual ~InternalTimer() = default;

    virtual std::string to_string() const = 0;
};