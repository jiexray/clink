#pragma once
#include "KeyedStateFactory.hpp"
#include "TemplateHelper.hpp"
#include "StateDescriptor.hpp"

/**
  Listener is given a callback when set_current_key is called (key context change).
*/
template <class K>
class KeySelectionListener {
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
public:
    virtual void key_selected(ConstParamK new_key) = 0;
};


/**
  A keyed state backend provides methods for managing keyed state.

  @param <K> The key by which state is keyed.
  @param <N> the type of the namespace
  @param <SV> the type of the stored state value
  @param <S> the type of the public API state
  @param <IS> the type of internal state
 */
template <class K, class N, class SV, class S, class IS>
class KeyedStateBackend: public KeyedStateFactory<N, SV, S, IS>
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
public:
    virtual void set_current_key(ConstParamK new_key) = 0;
    virtual ConstParamK get_current_key() const = 0;

    /**
      Creates or retrieves a keyed state backed by this state backend.

      @param state_descriptor The identifier for the state. This contains name and can create a default state value.

      @return A new key/value state backed by this backend.
     */
    virtual ParamS get_or_create_keyed_state(const StateDescriptor<S, SV>& state_descriptor) = 0;
    virtual ParamIS get_or_create_internal_keyed_state(const StateDescriptor<S, SV>& state_descriptor) = 0;

    /**
      Creates or retrieves a partitioned state backed by this state backend.

      @param state_descriptor The identifier for the state. This contains name and can create a default state value.

      @return A new key/value state backed by this backend.
     */
    virtual ParamS get_partitioned_state(ConstParamN ns, const StateDescriptor<S, SV>& state_descriptor) = 0;

    /**
      State backend will call KeySelectionListener::key_selected() when key context is switched if supported
     */
    virtual void register_key_selection_listener(KeySelectionListener<K>* listener) = 0;

    /**
      Stop calling listener registered in register_key_selection_listener.

      @return returns true iff listener was registered before.
     */
    virtual bool deregister_key_selection_listener(KeySelectionListener<K>* listener) = 0;
};

