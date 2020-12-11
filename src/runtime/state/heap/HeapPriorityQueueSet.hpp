#pragma once
#include "KeyGroupRange.hpp"
#include "KeyGroupRangeAssignment.hpp"
#include "TemplateHelper.hpp"
#include "LoggerFactory.hpp"
#include "StringUtils.hpp"

#include <map>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

/**
  A heap-based priority queue (minimum heap) with set semantics, based on std::priority_queue. The heap 
  is supported by has set for fast contains (de-depulicate) and deletes. Object identification happens 
  based operator ==.
 */
template <class T>
class HeapPriorityQueueSet {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<T>::type ParamT;
    typedef typename TemplateHelperUtil::ParamOptimize<T>::const_type ConstParamT;

    std::priority_queue<T> m_internal_min_heap;

    KeyGroupRange m_key_group_range;

    std::unordered_set<T>* m_deduplication_maps_by_key_group;

    // std::unordered_map<T, T>* m_deduplication_maps_by_key_group;

    int m_total_number_of_key_group;

    std::shared_ptr<spdlog::logger> m_logger = LoggerFactory::get_logger("HeapPriorityQueueSet");
public:
    HeapPriorityQueueSet(
            int minimum_capacity, 
            const KeyGroupRange& key_group_range, 
            int total_number_of_key_groups):
            m_total_number_of_key_group(total_number_of_key_groups),
            m_key_group_range(key_group_range) {
        int key_groups_in_local_range = key_group_range.get_number_of_key_groups();
        int deduplication_set_size = 1 + minimum_capacity / key_groups_in_local_range;
        m_deduplication_maps_by_key_group = new std::unordered_set<T>[key_groups_in_local_range];
    }

    ~HeapPriorityQueueSet() {
        delete[] m_deduplication_maps_by_key_group;
    }

    bool empty() {
        return m_internal_min_heap.empty();
    }

    /**
      Add the element to the queue. In constrast to the std::priority_queue and to maintain set semantics, this happens only if
      no such element is already contained.

      @return true if the operation changed the head element or if is it unclear if the head element changed. Only returns false
      iff the head element was not changed by this operator.
     */
    bool add(ConstParamT element) {
        if (get_dedup_map_for_element(element).insert(element).second) {
            SPDLOG_LOGGER_TRACE(m_logger, "Insert on element {}", StringUtils::to_string<T>(element));
            if (m_internal_min_heap.empty()) {
                m_internal_min_heap.push(element);
                return true;
            } else {
                ConstParamT previous_heap_head = m_internal_min_heap.top();
                m_internal_min_heap.push(element);
                return !(element < previous_heap_head);
            }
        } else {
            return false;
        }
        // m_internal_min_heap.push(element);
    }

    /**
      Pop out the head element in the priority queue.
     */
    void pop() {
        if (m_internal_min_heap.empty()) {
            throw std::runtime_error("Internal minimum heap is empty.");
        }

        ConstParamT heap_head = m_internal_min_heap.top();
        get_dedup_map_for_element(heap_head).erase(heap_head);

        m_internal_min_heap.pop();
    }

    /**
      Get the head element in the minimum heap.
     */
    ConstParamT top() {
        if (m_internal_min_heap.empty()) {
            throw std::runtime_error("Internal minimum heap is empty.");
        }

        return m_internal_min_heap.top();
    }

    std::unordered_set<T>& get_dedup_map_for_element(ConstParamT element) {
        // TODO: implement KeyExtractor, current, use the fisrt key_group in key_group_range
        int key_group = m_key_group_range.get_start_key_group();
        return m_deduplication_maps_by_key_group[key_group - m_key_group_range.get_start_key_group()];
    }

};

