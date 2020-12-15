#pragma once

#include "TimeWindow.hpp"
#include <map>

/**
  A hash map with key as TimeWindow.
 */
template <class K, class S>
class TimeWindowBasedHashMap {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::type ParamS;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<S>::const_type ConstParamS;

    long m_hash_bucket_size;

    long m_time_window_length;

    std::map<K, S>* m_internal_maps;

    std::map<K, S>& get_internal_map(const TimeWindow& window) {
        return m_internal_maps[(window.get_start() / m_time_window_length) % m_hash_bucket_size];
    }

public:
    TimeWindowBasedHashMap(long hash_bucket_size, long time_window_length):
            m_hash_bucket_size(hash_bucket_size),
            m_time_window_length(time_window_length) {
        m_internal_maps = new std::map<K, S>[hash_bucket_size];
    }

    ~TimeWindowBasedHashMap() {
        delete[] m_internal_maps;
    }

    std::map<K, S>& operator [](const TimeWindow& window) {
        return m_internal_maps[(window.get_start() / m_time_window_length) % m_hash_bucket_size];
        // return get_internal_map(window);
        // return m_internal_maps[(window.get_start() / m_time_window_length) % m_hash_bucket_size];
    }

    std::map<K, S>& get_map(const TimeWindow& window) {
        return m_internal_maps[(window.get_start() / m_time_window_length) % m_hash_bucket_size];
    }

    void remove(const TimeWindow& window, ConstParamK key) {
        std::map<K, S>& window_map = get_internal_map(window);

        if (window_map.find(key) == window_map.end()) {
            return;
        }

        window_map.erase(key);
    }
};
