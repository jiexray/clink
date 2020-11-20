#pragma once

/**
  Base interface for Reduce functions. Reduce functions combine groups of elements to 
  a single value, by taking always two elements and combining them into one. Reduce functions 
  may be used on entire data set, or on grouped data sets. In the latter case, each group is reduced
  individually.
 */
template <class T>
class ReduceFunction
{
public:
    /**
      The core method of ReduceFunction, combining two values into one value of the same type.
      The reduce function is consecutively applied to all values of a group until only a single value remains.

      @param value1 The first value to combine.
      @param value2 The second value to combine.
      @return The conbimed value of both input values.
     */
    virtual T* reduce(T* value1, T* value2) = 0;
};
