/**
 * This implement of StateMap uses nested HashMap objects.
 * 
 * @param <K>: type of key.
 * @param <N>: type of ns.
 * @param <S>: type of value.
 */
#pragma once

#include "StateMap.hpp"
#include "StringUtils.hpp"
#include "TimeWindowBasedHashMap.hpp"
#include <map>
#include <unordered_map>

template <class K, class N, class S>
class NestedStateMap: public StateMap<K, N, S>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;

    std::map<N, std::map<K, S>> m_ns_map;
    typedef typename std::map<N, std::map<K, S>> nsMap;
    typedef typename nsMap::iterator nsMapIter;
public:

    NestedStateMap() {}

    // ----------------------------------------------------
    //  Public API from StateMap
    // ----------------------------------------------------
    int size() override {
        int count = 0;
        nsMapIter it = this->m_ns_map.begin();
        while(it != this->m_ns_map.end()) {
            count += (it->second).size();
            ++it;
        }
        return count;
    }

    ParamS get(ConstParamK key, ConstParamN ns) override {
        return m_ns_map[ns][key];
    }

    // TODO: fix the #contains_key(), stl containers do not need this function.
    bool contains_key(ConstParamK key, ConstParamN ns) override {
        return true;
    }

    void put(ConstParamK key, ConstParamN ns, ConstParamS state) {
        this->m_ns_map[ns][key] = state;
    }

    S put_and_get_old(ConstParamK key, ConstParamN ns, ConstParamS state) override {
        if (m_ns_map[ns].find(key) == m_ns_map[ns].end()) {
            throw std::invalid_argument("Unknown <key, namespace>: (" + StringUtils::to_string<K>(key) + ", " + StringUtils::to_string<N>(ns) + ")");
        }

        S old_val = this->m_ns_map[ns][key];

        this->m_ns_map[ns][key] = state;

        return old_val;
    }

    void remove(ConstParamK key, ConstParamN ns) override {
        remove_and_get_old(key, ns);
    }

    S remove_and_get_old(ConstParamK key, ConstParamN ns) override {
        if (m_ns_map[ns].find(key) == m_ns_map[ns].end()) {
            throw std::invalid_argument("Unknown <key, namespace>: (" + StringUtils::to_string<K>(key) + ", " + StringUtils::to_string<N>(ns) + ")");
        }

        S old_val = this->m_ns_map[ns][key];

        this->m_ns_map[ns].erase(key);

        if (this->m_ns_map[ns].empty()) {
            this->m_ns_map.erase(ns);
        }

        return old_val;
    }

    template <class T>
    void transform(
            ConstParamK key, 
            ConstParamN ns, 
            typename TemplateHelperUtil::ParamOptimize<T>::const_type value, 
            StateTransformationFunction<S, T>& transformation) {
        std::map<K, S>& map_for_ns = this->m_ns_map[ns];
        S& pre_val = map_for_ns[key];
        pre_val = transformation.apply(&pre_val, value);
    }
};