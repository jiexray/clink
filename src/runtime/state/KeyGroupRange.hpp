#pragma once
#include <assert.h>
#include <algorithm>
#include <iostream>

/**
This class defines a range of key-group indexes. Key-groups are the granularity into which the keyspace of a job
is partitioned for keyed state-handling in state backends. The boundaries of the range are inclusive.
 */
class KeyGroupRange
{
private:
    int         m_start_key_group;
    int         m_end_key_group;

    /**
      Empty KeyGroup Constructor.
     */
    KeyGroupRange() {
        m_end_key_group = 0;
        m_end_key_group = -1;
    }
public:
    KeyGroupRange(int start_key_group, int end_key_group) {
        assert(start_key_group <= end_key_group);
        assert(start_key_group >= 0);

        m_start_key_group = start_key_group;
        m_end_key_group = end_key_group;
    }

    KeyGroupRange(const KeyGroupRange& range) {
        m_start_key_group = range.m_start_key_group;
        m_end_key_group = range.m_end_key_group;
    }

    /**
      Check whether or not a single key-group is contains in the range.

      @param key_group: Key-group to check for inclusion.
      @return True, only if the key-group is in the range.
     */
    bool contains(int key_group) const {
        return key_group >= m_start_key_group && key_group <= m_end_key_group;
    }

    /**
      Create a range that represent the intersection between this range and the given range.

      @param other A keyGroupRange to intersect.
      @return Key-group range that is the intersection between this and the given key-group range.
     */
    KeyGroupRange get_intersection(const KeyGroupRange & other) {
        int start = std::max(m_start_key_group, other.m_start_key_group);
        int end = std::min(m_end_key_group, other.m_end_key_group);

        return start <= end ? KeyGroupRange(start, end) : KeyGroupRange();
    }

    /**
      @return The number of key-groups in the range.
     */
    int get_number_of_key_groups() const {
        return 1 + m_end_key_group - m_start_key_group;
    }

    /**
      @return The first key-group in the range.
     */
    int get_start_key_group() const {
        return m_start_key_group;
    }

    /**
      @return The last key-group in the range.
     */        
    int get_end_key_group() const {
        return m_end_key_group;
    }

    int get_key_group_id(int idx) const {
        if (idx < 0 || idx > get_number_of_key_groups()) {
            throw std::invalid_argument("Key group index out of bounds: " + std::to_string(idx));
        }
        return m_start_key_group + idx;
    }

    std::string to_string() const {
        return "KeyGroupRange{start_key_group=" + std::to_string(m_start_key_group) +
            ", end_key_group=" + std::to_string(m_end_key_group) + 
            "}";
    }
};

