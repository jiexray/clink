#pragma once
#include "State.hpp"
#include "TemplateHelper.hpp"
#include <map>

/**
  State interface for partitioned key-value state. The key-value pair can be added, updated and retieved.

  @param <UK> Type of the keys in the state.
  @param <UV> Type of the values in the state.
 */
template <class UK, class UV>
class MapState: virtual public State
{
private: 
    typedef typename TemplateHelperUtil::ParamOptimize<UK>::type ParamUK;
    typedef typename TemplateHelperUtil::ParamOptimize<UV>::type ParamUV;
    typedef typename TemplateHelperUtil::ParamOptimize<UK>::const_type ConstParamUK;
    typedef typename TemplateHelperUtil::ParamOptimize<UV>::const_type ConstParamUV;
public:
    /**
      Returns the current value associated with the given key.

      @param key the key of the mapping.
      @return The value of the mapping with the given key.
     */
    virtual ConstParamUV get(ConstParamUK key) = 0;

    /**
      Associate a new value with the given key.

      @param key The key of the mapping.
      @param value The new value of the mapping.
     */
    virtual void put(ConstParamUK key, ConstParamUV value) = 0;

    /**
      Copies all the mappings from the given map into the state.

      @param map The mapping to be stored in this state.
     */
    virtual void put_all(const std::map<UK, UV>& value) = 0;

    /**
      Deletes the mapping of the given key.

      @param key the key of the mapping.
     */
    virtual void remove(ConstParamUK key) = 0;

    /**
      Returns whether there exists the given mapping.

      @param key the key of the mapping
      @return True if there exists a mapping whose key equals to the given key
     */
    virtual bool contains(ConstParamUK key) = 0;

    /**
      Returns true if this state contains no key-value mappings, otherwise false.

      @return Ture if this state contains no key-value mappings, otherwise false.
     */
    virtual bool is_empty() = 0;

};

