#pragma once
#include "InternalTimer.hpp"
#include <string>
#include <functional>
#include "StringUtils.hpp"

/**
  Implementation of InternalTimer to use with a HeapPriorityQueueSet.
 */
template <class K, class N>
class TimerHeapInternalTimer: public InternalTimer<K, N>
{
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;

    K m_key;
    N m_namespace;
    long m_timestamp;
public:
    TimerHeapInternalTimer(long timestamp, ConstParamK key, ConstParamN ns): 
            m_timestamp(timestamp),
            m_key(key),
            m_namespace(ns) {} 
    
    TimerHeapInternalTimer(const TimerHeapInternalTimer& other):
    m_key(other.m_key),
    m_namespace(other.m_namespace),
    m_timestamp(other.m_timestamp) {}

    // ~TimerHeapInternalTimer() {}

    long get_timestamp() const override  {
        return m_timestamp;
    }

    ConstParamK get_key() const override {
        return m_key;
    }

    ConstParamN get_namespace() const override {
        return m_namespace;
    }

    bool operator==(const TimerHeapInternalTimer& other) const {
        if (this == &other) {
            return true;
        }

        return m_timestamp == other.m_timestamp 
                && m_key == other.m_key
                && m_namespace == other.m_namespace;
    }

    TimerHeapInternalTimer& operator=(const TimerHeapInternalTimer& other) {
        if (this != &other) {
           this->m_key = other.m_key;
           this->m_namespace = other.m_namespace;
           this->m_timestamp = other.m_timestamp;
        }  
        return *this;
    }

    TimerHeapInternalTimer& operator=(TimerHeapInternalTimer&& other) {
        if (this != &other) {
           this->m_key = other.m_key;
           this->m_namespace = other.m_namespace;
           this->m_timestamp = other.m_timestamp;
        }  
        return *this;
    }

    bool operator< (const TimerHeapInternalTimer& other) const {
        if (this == &other) {
            return false;
        }

        return this->m_timestamp > other.m_timestamp;
    }

    std::string to_string() const override {
        return "Timer{timestamp=" + std::to_string(m_timestamp) + 
                ", key=" + StringUtils::to_string<K>(m_key) + 
                ", namespace=" + m_namespace.to_string() +
                "}";
    }

};

namespace std {
    template <class K, class N>
    struct hash<TimerHeapInternalTimer<K, N>> {
        std::size_t operator()(const TimerHeapInternalTimer<K, N>& obj) const {
            return std::hash<std::string>()(obj.to_string());
        } 
    };
}

// namespace StringUtils {
//     template <class K, class N>
//     inline std::string to_string<TimerHeapInternalTimer<K, N>>(const TimerHeapInternalTimer<K, N>& timer) {
//         return timer.to_string();
//     }
// }

