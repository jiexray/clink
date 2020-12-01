#pragma once

#include "InternalKvState.hpp"
#include "MapState.hpp"
#include <map>

/**
  The peer to the MapState in the internal state type hierarchy.

  @param <K>: type of key
  @param <N>: type of ns
  @param <UK>: Type of the values folded into the state
  @param <UV>: Type of the value in the state
 */
template <class K, class N, class UK, class UV>
class InternalMapState: public MapState<UK, UV>, public virtual InternalKvState<K, N, std::map<UK, UV>>{
//     private: 
//     typedef typename TemplateHelperUtil::ParamOptimize<UK>::type ParamUK;
//     typedef typename TemplateHelperUtil::ParamOptimize<UV>::type ParamUV;
//     typedef typename TemplateHelperUtil::ParamOptimize<UK>::const_type ConstParamUK;
//     typedef typename TemplateHelperUtil::ParamOptimize<UV>::const_type ConstParamUV;
// public:
//     ConstParamUV get(ConstParamUK key) override {return MapState<UK, UV>::get(key);};

//     void put(ConstParamUK key, ConstParamUV value) override {MapState<UK, UV>::put(key, value)};

//     void put_all(const std::map<UK, UV>& value) override {MapState<UK, UV>::put_all(value);};

//     void remove(ConstParamUK key) override {MapState<UK, UV>::remove(key);};

//     bool contains(ConstParamUK key) override {return MapState<UK, UV>::contains(key);};

//     virtual bool is_empty() override {return MapState<UK, UV>::is_empty()};
public:
};

