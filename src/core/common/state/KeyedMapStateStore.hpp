#pragma once
#include "MapState.hpp"
#include "MapStateDescriptor.hpp"
#include <map>
#include <iostream>

class MapStateGetter {
public:
    virtual void operator()(const StateDescriptorBase&, State*) const = 0;
};

template <class UK, class UV, class KeyedMapStateStoreType>
class MapStateGetterImpl: public MapStateGetter {
public:
    KeyedMapStateStoreType& m_data;
    MapStateGetterImpl(KeyedMapStateStoreType& data): m_data(data) {}
    virtual void operator()(const StateDescriptorBase& state_properties, State* ret_state) const override {
        m_data.get_map_state_impl(*((MapStateDescriptor<UK, UV>*)(&state_properties)), dynamic_cast<MapState<UK, UV>*>(ret_state));
    }
};

/**
  This interface contains method for registering keyed state with managed store.
 */
class KeyedMapStateStore
{
protected:
    KeyedMapStateStore(MapStateGetter& map_state_getter): get_map_state(map_state_getter){}
public:
    MapStateGetter& get_map_state;
    virtual ~KeyedMapStateStore() {
        delete &get_map_state;
    }
};

template <typename UK, typename UV>
class TestMapKeyedMapStateStore: public KeyedMapStateStore{
private:
    void get_map_state_impl(const MapStateDescriptor<UK, UV>& state_properties, MapState<UK, UV>* ret_state) {
        std::cout << "TestMapKeyedMapStateStore::get_map_state_impl()" << std::endl;
    }
public:
    friend class MapStateGetterImpl<UK, UV, TestMapKeyedMapStateStore<UK, UV>>;

    TestMapKeyedMapStateStore(): KeyedMapStateStore(*(new MapStateGetterImpl<UK, UV, TestMapKeyedMapStateStore<UK, UV>>(*this))) {}
    virtual ~TestMapKeyedMapStateStore() {}
};
