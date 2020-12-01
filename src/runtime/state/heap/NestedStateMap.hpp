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
#include <map>

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

    std::map<N, std::map<K, S>>     m_ns_map;
    typedef typename std::map<N, std::map<K, S>> nsMap;
    typedef typename nsMap::iterator nsMapIter;
public:

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
        if (this->m_ns_map.find(ns) != this->m_ns_map.end()) {
            if (m_ns_map[ns].find(key) == m_ns_map[ns].end()) {
                throw std::invalid_argument("Unknown key: " + StringUtils::to_string<K>(key));
            } else {
                return m_ns_map[ns][key];
            }
        } else {
            throw std::invalid_argument("Unknown namespace: " + StringUtils::to_string<N>(ns));
        }
    }

    bool contains_key(ConstParamK key, ConstParamN ns) override {
        if (this->m_ns_map.find(ns) != this->m_ns_map.end()) {
            if (m_ns_map[ns].find(key) == m_ns_map[ns].end()) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    void put(ConstParamK key, ConstParamN ns, ConstParamS state) {
        if (this->m_ns_map.find(ns) == this->m_ns_map.end()) {
            this->m_ns_map[ns] = std::map<K, S>();
        }
        // auto ret = this->m_ns_map[ns].insert(std::make_pair(key, state));
        // if (ret.second == false) {
        //     this->m_ns_map[ns][key] = state;
        // }
        this->m_ns_map[ns][key] = state;
    }

    ParamS put_and_get_old(ConstParamK key, ConstParamN ns, ConstParamS state) override {
        if (contains_key(key, ns) == false) {
            throw std::invalid_argument("Unknown <key, namespace>: (" + StringUtils::to_string<K>(key) + ", " + StringUtils::to_string<N>(ns) + ")");
        }

        ParamS old_val = this->m_ns_map[ns][key];

        this->m_ns_map[ns].insert(std::make_pair(key, state));

        return old_val;
    }

    void remove(ConstParamK key, ConstParamN ns) override {
        remove_and_get_old(key, ns);
    }

    ParamS remove_and_get_old(ConstParamK key, ConstParamN ns) override {
        if (!contains_key(key, ns)) {
            throw std::invalid_argument("Unknown <key, namespace>: (" + StringUtils::to_string<K>(key) + ", " + StringUtils::to_string<N>(ns) + ")");
        }

        ParamS old_val = this->m_ns_map[ns][key];

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
        if (this->m_ns_map.find(ns) == this->m_ns_map.end()) {
            this->m_ns_map[ns] = std::map<K, S>();
        }

        S* new_state;

        if (this->m_ns_map[ns].find(key) != this->m_ns_map[ns].end()) {
            new_state = transformation.apply(&this->m_ns_map[ns][key], value);
        } else {
            new_state = transformation.apply(nullptr, value);
        }

        this->m_ns_map[ns][key] = *new_state;
        delete new_state;
    }
};