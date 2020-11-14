#pragma once

#include "State.hpp"
#include "TemplateHelper.hpp"


/**
  The InternalKvState is the root of the internal state type hierarchy, similar to the State 
  being the root of the public API state hierarchy.

  The internal state classes give access to the namespace getters and setters and access to 
  additional functionality, like raw value access or state merging.

  The public API state hierarchy is intended to be programmed against by Flink application.
  The internal state hierarchy holds all the auxiliary methods that are used by the runtime and not 
  intended to be used by user applications. These internal methods are considered of limited use to 
  users and only confusing, and are usually no regarded as stable across releases.

  @param <K>: type of key
  @param <N>: type of ns
  @param <S>: type of state
 */
template<class K, class N, class S>
class InternalKvState: virtual public State
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;
public:
    /**
      Sets the current namespace, which will be used when using the state access methods.

      @param ns The namespace.
     */
    virtual void set_current_namespace(ConstParamN ns) = 0;
};

