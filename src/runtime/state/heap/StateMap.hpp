/**
 * Base class for state maps.
 */
#pragma once
#include "TemplateHelper.hpp"
#include <type_traits>


/**
  @param <K>: type of key
  @param <N>: type of ns
  @param <S>: type of state
 */
template <class K, class N, class S>
class StateMap
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
      @return if this StateMap has no elements, false; otherwise, size()
     */
    bool                is_empty() {return size() == 0;}

    /**
      @return the total number of entries in this StateMap.
     */
    virtual int         size() = 0;

    /**
      @param key:       the key. Not null.
      @param ns: the ns. Not null.
      @return the state of the mapping with the specified key/ns composite key, or null
      if no mapping for the specified key is found.
     */
    virtual ParamS      get(ConstParamK key, ConstParamN ns) = 0;

    /**
      @param key:       the key in the composite key to search for. Not null.
      @param ns: the ns in the composite key to search for. Not null.
      @return true if this map contains the specified key/ns composite key, 
      false otherwise.
     */
    virtual bool        contains_key(const ParamK key, ConstParamN ns) = 0;

    /**
      Maps the specified key/ns composite key to the specified value. This method should be preferred
      over put_and_get_old(ns, State) when the caller is not interested in the old state.
      @param key:       the key. Not null.
      @param ns: the ns. Not null.
      @param state:     the state. Can be null.
     */
    virtual void        put(const ParamK key, ConstParamN ns, ConstParamS state) = 0;

    /**
      Maps the composite of active key and given ns to the specified state. Returns the previous state that
      was registered under the composite key.
      
      @param key:       the key. Not null.
      @param ns: the ns. Not null.
      @param state:     the state. Can be null.
      @return the state of any previous mapping with the specified key or null if there was no such mapping.
     */
    virtual ParamS      put_and_get_old(const ParamK key, ConstParamN ns, ConstParamS state) = 0;

    /**
      Removes the mapping for the composite of active key and given ns. This method should be preferred 
      over remove_and_get_old() when the caller is not interested in the old state.
      
      @param key:       the key. Not null.
      @param ns: the ns. Not null.
     */
    virtual void        remove(const ParamK key, ConstParamN ns) = 0;


    /**
     Removes the mapping for the composite of active key and given ns, returning the state that was found 
     under the entry.

     @param key:        the key of the mapping to remove. Not null.
     @param ns:  the ns of the mapping to remove. Not null.
     @return the state of the removed mapping of null if no mapping 
     for the specified key was found.
     */
    virtual ParamS      remove_and_get_old(const ParamK key, ConstParamN ns) = 0;
};
