#pragma once
#include "InternalKeyContext.hpp"
#include "StateMap.hpp"
#include "NestedStateMap.hpp"
#include "StateTransformationFunction.hpp"

/**
  Base class for state tables. Accesses to state are typically scoped by the currently active key, as provided
  through the InternalKeyContext.

  @param <K> type of key
  @param <N> type of namespace
  @param <S> type of state
 */
template <class K, class N, class S>
class StateTable
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;
protected:
    InternalKeyContext<K>&          m_key_context;
    int                             m_key_group_offset;
    StateMap<K, N, S>**             m_key_grouped_state_maps;
    int                             m_key_grouped_state_maps_length;

public:
    StateTable(InternalKeyContext<K>& key_context): m_key_context(key_context) {
        m_key_group_offset = m_key_context.get_key_group_range().get_start_key_group();

        m_key_grouped_state_maps_length = m_key_context.get_key_group_range().get_number_of_key_groups();
        this->m_key_grouped_state_maps = new StateMap<K, N, S>*[m_key_grouped_state_maps_length];
    }

    virtual StateMap<K, N, S>* create_state_map() = 0;

    ~StateTable() {
        for (int i = 0; i < m_key_grouped_state_maps_length; i++) {
            delete m_key_grouped_state_maps[i];
        }
        delete[] m_key_grouped_state_maps;
    }

    // ------------------------------------------------------------------
    //  Main interface methods of StateTable
    // ------------------------------------------------------------------

    /**
      Return whether this StateTable is empty.

      @return true if this StateTable has no elements, false otherwise.
     */
    bool is_empty() {
        return size() == 0;
    }

    /**
      Returns the total number of entries in this StateTable. This is the sum of both sub-tables.

      @return the number of entries in this StateTable.
     */
    int size() {
        int count = 0;
        for (int i = 0; i < m_key_grouped_state_maps_length; i++) {
            count += this->m_key_grouped_state_maps[i]->size();
        }
        return count;
    }

    /**
      Returns the state of the mapping for the composite of active key and given namespace.

      @param ns the namespace. Not null.
      @return the states of the mapping with the specified key/namespace composite key, or null
      if no mapping of the specified key is found.

      Note: this function can only be used by State. User cannot use it.
     */
    ParamS get(ConstParamN ns) {
        return get(m_key_context.get_current_key(), m_key_context.get_current_key_group_index(), ns);
    }

    /**
      Returns the state of the mapping for the composite of active key and given namespace.

      @param ns the namespace. Not null.
      @return true if this map contains the specified key/namespace componsite key,
      false otherwise.
     */
    bool contains_key(ConstParamN ns) {
        return contains_key(m_key_context.get_current_key(), m_key_context.get_current_key_group_index(), ns);
    }

    /**
      Maps the composite of active key and given namespace to the specified state.

      @param ns     the namespace. Not null.
      @param state  the state. Can be null.
     */
    void put(ConstParamN ns, ConstParamS state) {
        put(m_key_context.get_current_key(), m_key_context.get_current_key_group_index(), ns, state);
    }

    void put(ConstParamK key, int key_group, ConstParamN ns, ConstParamS state) {
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group);

        state_map->put(key, ns, state);
    }

    /**
      Removes the mapping for the composite of active key and given namespace. This method should be preferred over
      remove_and_get_old() when the caller is not interested in the old state.

      @param ns the namespace of the mapping to remove. Not null.
     */
    void remove(ConstParamN ns) {
        remove(m_key_context.get_current_key(), m_key_context.get_current_key_group_index(), ns);
    }

    /**
      Removes the mapping for the composite of active key and given namespace, returning the state that was
      found under the entry.

      @param ns the namespace of the mapping to remove. Not null.
      @return the state of the removed mapping, if not exist, it will throw exception. Use contains_key before
      calling this method.
     */
    ConstParamS remove_and_get_old(ConstParamN ns) {
        return remove_and_get_old(m_key_context.get_current_key(), m_key_context.get_current_key_group_index(), ns);
    }

    // ------------------------------------------------------------------
    //  For queryable state
    // ------------------------------------------------------------------
    
    /**
      Returns the state for the composite of active key and given ns. This is typically used by queryable state.

      @param key the key. Not null.
      @param ns  the namespace. Not null.
      @return the state of the mapping with the specified key/namespace composite key. Or throw exception if no mapping
      for the specified key is found.
     */
    ConstParamS get(ConstParamK key, ConstParamN ns) {
        // TODO
    }

    template <class T>
    void transform(
            ConstParamN ns, 
            typename TemplateHelperUtil::ParamOptimize<T>::const_type value, 
            StateTransformationFunction<S, T>& transformation) {
        ConstParamK key = m_key_context.get_current_key();

        int key_group = m_key_context.get_current_key_group_index();
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group);

        if (state_map == nullptr) {
            throw std::runtime_error("key_group_index " + std::to_string(key_group) + " is out of scope");
        }
        if (dynamic_cast<NestedStateMap<K, N, S>*>(state_map) != nullptr) {
            dynamic_cast<NestedStateMap<K, N, S>*>(state_map)->transform<T>(key, ns, value, transformation);
        } else {
            throw std::runtime_error("StateMap type unknown");
        }
    }

    // ------------------------------------------------------------------
    //  Internal interfaces
    // ------------------------------------------------------------------
private:
    ParamS get(ConstParamK key, int key_group_index, ConstParamN ns)  {
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group_index);

        if (state_map == nullptr) {
            throw std::runtime_error("key_group_index " + std::to_string(key_group_index) + " is out of scope");
        }

        state_map->get(key, ns);
    }

    bool contains_key(ConstParamK key, int key_group_index, ConstParamN ns) {
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group_index);

        return state_map != nullptr && state_map->contains_key(key, ns);
    }

    void remove(ConstParamK key, int key_group_index, ConstParamN ns) {
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group_index);

        state_map->remove(key, ns);
    }

    ConstParamS remove_and_get_old(ConstParamK key, int key_group_index, ConstParamN ns) {
        StateMap<K, N, S>* state_map = get_map_for_key_group(key_group_index);

        return state_map->remove_and_get_old(key, ns);
    }

    // ------------------------------------------------------------------
    //  Accessing to maps
    // ------------------------------------------------------------------
public: 
    int index_to_offset(int index) {
        return index - m_key_group_offset;
    }

    StateMap<K, N, S>* get_map_for_key_group(int key_group_index) {
        int pos = index_to_offset(key_group_index);
        if (pos >= 0 && pos < m_key_grouped_state_maps_length) {
            return m_key_grouped_state_maps[pos];
        } else {
            return nullptr;
        }
    }
};

