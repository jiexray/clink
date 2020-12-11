#pragma once
#include <functional>
#include <assert.h>
#include "TemplateHelper.hpp"

class KeyGroupRangeAssignment
{
public:
    template <class K>
    static int assign_to_key_group(typename TemplateHelperUtil::ParamOptimize<K>::const_type key, int max_parallelism) {
        std::size_t h = std::hash<K>{}(key);
        return compute_key_group_for_key_hash(h, max_parallelism);
    }

    static int compute_key_group_for_key_hash(std::size_t key_hash, int max_parallelism) {
        return key_hash % max_parallelism;
    }

    static KeyGroupRange compute_key_group_range_for_operator_index(int max_parallelism, int parallelism, int operator_index) {
        assert(max_parallelism >= parallelism);
        int start = ((operator_index * max_parallelism + parallelism - 1) / parallelism);
        int end = ((operator_index + 1) * max_parallelism - 1) / parallelism;
        return KeyGroupRange(start, end);
    }
};

