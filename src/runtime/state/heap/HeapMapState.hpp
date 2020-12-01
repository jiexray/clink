#pragma once
#include "AbstractHeapState.hpp"
#include "InternalMapState.hpp"
#include "StringUtils.hpp"
#include "StateDescriptor.hpp"

/**
  Heap-backed partitioned MapState that is snappshotted into files.

  @param <K>: type of key
  @param <N>: type of ns
  @param <UK>: Type of the values folded into the state
  @param <UV>: Type of the value in the state
 */
template <class K, class N, class UK, class UV>
class HeapMapState: public AbstractHeapState<K, N, std::map<UK, UV>>, public InternalMapState<K, N, UK, UV>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<UK>::type ParamUK;
    typedef typename TemplateHelperUtil::ParamOptimize<UV>::type ParamUV;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<UK>::const_type ConstParamUK;
    typedef typename TemplateHelperUtil::ParamOptimize<UV>::const_type ConstParamUV;
public:
    HeapMapState() {
        throw std::runtime_error("Not implement HeapMapState()");
    }

    // ~HeapMapState() {
    //     std::cout << "HeapMapState dtor()" << std::endl;
    // }

    HeapMapState(
            StateTable<K, N, std::map<UK, UV>>& state_table,
            const std::map<UK, UV>& default_value):
            AbstractHeapState<K, N, std::map<UK, UV>>(state_table, default_value) {}

    ConstParamUV get(ConstParamUK key) override {
        std::map<UK, UV>& user_map = this->m_state_table.get(this->m_current_namespace);

        if (user_map.find(key) == user_map.end()) {
            throw std::runtime_error("Unknown key: " + StringUtils::to_string<K>(key)  + " for HeapMapState");
        }
        return user_map[key];
    }

    void put(ConstParamUK user_key, ConstParamUV user_value) override {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            this->m_state_table.put(this->m_current_namespace, std::map<UK, UV>());
            this->m_state_table.get(this->m_current_namespace).insert(std::make_pair(user_key, user_value));
        } else {
            auto res = this->m_state_table.get(this->m_current_namespace).insert(std::make_pair(user_key, user_value));
            if (res.second == false) {
                this->m_state_table.get(this->m_current_namespace)[user_key] = user_value;
            }
        }
    }

    void put_all(const std::map<UK, UV>& value) override {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            this->m_state_table.put(this->m_current_namespace, std::map<UK, UV>());
        }
        std::map<UK, UV>& user_map = this->m_state_table.get(this->m_current_namespace);

        for(auto & it: value) {
            if (user_map.find(it.first) != user_map.end()) {
                user_map[it.first] = it.second;
            } else {
                user_map.insert(std::make_pair(it.first, it.second));
            }
        }
    }

    void remove(ConstParamUK key) override {
        std::map<UK, UV>& user_map = this->m_state_table.get(this->m_current_namespace);

        if (user_map.find(key) == user_map.end()) {
            throw std::runtime_error("Unknown key: " + StringUtils::to_string<K>(key)  + " for HeapMapState");
        }

        user_map.erase(key);

        if (user_map.empty()) {
            this->clear();
        }
    }

    bool contains(ConstParamUK key) override {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            return false;
        }
        std::map<UK, UV>& user_map = this->m_state_table.get(this->m_current_namespace);
        return user_map.find(key) != user_map.end();
    }
    
    bool is_empty() override {
        if (!this->m_state_table.contains_key(this->m_current_namespace)) {
            return true;
        }
        std::map<UK, UV>& user_map = this->m_state_table.get(this->m_current_namespace);

        return user_map.empty();
    }

    void set_current_namespace(ConstParamN ns) override {
        this->m_current_namespace = ns;
    }

    template<class IS>
    static IS* create(const StateDescriptor<MapState<UK, UV>, std::map<UK, UV>>& state_desc, StateTable<K, N, std::map<UK, UV>>& state_table) {
        return (IS*)new HeapMapState(state_table, state_desc.get_default_value());
    }
};
