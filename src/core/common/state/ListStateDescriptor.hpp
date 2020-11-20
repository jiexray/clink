#pragma once
#include "StateDescriptor.hpp"
#include "ListState.hpp"
#include <vector>

/**
  A StateDescriptor for ListState. This can be used to create state where the type 
  is a list that can be appended and iterated over.
 */
template <class T>
class ListStateDescriptor: public StateDescriptor<ListState<T>, std::vector<int>> {
public:
    ListStateDescriptor(std::string name):
        StateDescriptor<ListState<T>, std::vector<int>>(name, std::vector<T>()) {}

    StateDescriptorType get_type() override {
        return StateDescriptorType::LIST;
    }
};

