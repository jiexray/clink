#pragma once
#include "ListState.hpp"
#include "InternalListState.hpp"
#include "ListStateDescriptor.hpp"
#include <vector>
#include <iostream>

class ListStateGetter {
public:
    virtual void operator() (const StateDescriptorBase&, State*) const = 0;
};

template <class T, class KeyedListStateStoreType>
class ListStateGetterImpl: public ListStateGetter {
public:
    KeyedListStateStoreType& m_data;
    ListStateGetterImpl(KeyedListStateStoreType& data): m_data(data) {}
    virtual void operator() (const StateDescriptorBase& state_properties, State* ret_state) const override{
        m_data.get_list_state_impl(
            *((ListStateDescriptor<T>*)(&state_properties)),
            dynamic_cast<ListState<T>*>(ret_state));
    }
};

/**
 This interface contains method for registering keyed ListState with managed store.
 */
class KeyedListStateStore
{
protected:
    KeyedListStateStore(ListStateGetter& list_state_getter): get_list_state(list_state_getter) {}
public:
    ListStateGetter& get_list_state;
    virtual ~KeyedListStateStore() {
        delete &get_list_state;
    }
};

