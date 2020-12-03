#pragma once
#include "StateDescriptor.hpp"
#include "ListState.hpp"
#include <vector>

/**
  A StateDescriptor for ListState. This can be used to create state where the type 
  is a list that can be appended and iterated over.
 */
template <class T>
class ListStateDescriptor: 
        public StateDescriptor<ListState<T>, std::vector<T>>,
        public StateDescriptor<AppendingState<T, std::vector<T>>, std::vector<T>> {
    typedef typename TemplateHelperUtil::ParamOptimize<T>::type ParamT;
    typedef typename TemplateHelperUtil::ParamOptimize<T>::const_type ConstParamT;
public:
    ListStateDescriptor(std::string name):
        StateDescriptor<ListState<T>, std::vector<T>>(name, std::vector<T>()),
        StateDescriptor<AppendingState<T, std::vector<T>>, std::vector<T>>(name, std::vector<T>()) {}

    StateDescriptorType get_type() override {
        return StateDescriptorType::LIST;
    }

    /* Properties */
    std::string get_name() const{
        return StateDescriptor<ListState<T>, std::vector<T>>::get_name();
    }

    ConstParamT get_default_value() const {
        return StateDescriptor<ListState<T>, std::vector<T>>::get_default_value();
    }
};

