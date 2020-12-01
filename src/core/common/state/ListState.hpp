#pragma once
#include "AppendingState.hpp"
#include <vector>

/**
  State interface for partitioned list state in Operations.
 */
template <class T>
class ListState: public AppendingState<T, std::vector<T>>
{
public:
    /**
      Updates the operator state accessible by updating existing values to 
      the given list of values. The next time get() is called the returned 
      state will represent the updated list.
     */
    virtual void update(const std::vector<T>& values) = 0;

    /**
      Updates the operator state accessible by get() by adding the given 
      values to existing list of values. 
     */
    virtual void add_all(const std::vector<T>& values) = 0;

    /**
      Returns whether there is a list in the state.
     */
    virtual bool contains_list() = 0;
};
