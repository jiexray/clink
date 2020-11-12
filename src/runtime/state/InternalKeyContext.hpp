#pragma once
#include "KeyGroupRange.hpp"

/**
  This interface is the current context of a keyed state. It provides information about the currently selected key in 
  the context, the corresponding key-group, and other key and key-grouping related information.

  The typical use case for this interface is providing a view on the current-key selection aspects of KeyStateBackend.
 */
template <class K>
class InternalKeyContext
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
public:
    /**
      Used by states to access the current key.
     */
    virtual ConstParamK     get_current_key() = 0;

    /**
      Returns the key-group to which the current key belongs.
     */
    virtual int             get_current_key_group_index() = 0;
    
    /**
      Returns the number of key-groups aka max parallelism.
     */
    virtual int             get_number_of_key_groups() = 0;
    
    /**
      Return the key groups for this backend.
     */
    virtual const KeyGroupRange&   
                            get_key_group_range() = 0;

    /**
      Set current key of the context.

      @param current_key: the current key to set to.
     */
    virtual void            set_current_key(ConstParamK current_key) = 0;

    /**
      Set current key group index of the context.

      @param current_key_group_index: the current key group index to set to.
     */
    virtual void            set_current_key_group_index(int current_key_group_index) = 0;
};

