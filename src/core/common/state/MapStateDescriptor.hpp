#pragma once
#include "StateDescriptor.hpp"
#include "MapState.hpp"

/**
  A StateDescriptor for MapState. This can be used to create state where the type 
  is a map that can be updated and iterated over.

  @param <UK> The type of the keys that can be added to the map state.
  @param <UV> The type of the values 
 */
template<class UK, class UV>
class MapStateDescriptor: public StateDescriptor<MapState<UK, UV>, std::map<UK, UV>>
{
public:
    MapStateDescriptor(const std::string& name): StateDescriptor<MapState<UK, UV>, std::map<UK, UV>>(name, std::map<UK, UV>()){}

    StateDescriptorType get_type() override {
        return StateDescriptorType::MAP;
    }

    std::string get_state_descriptor_id() const {
        return std::string(typeid(MapStateDescriptor<UK, UV>).name());
    }
};

