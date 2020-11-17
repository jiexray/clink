#pragma once
#include <functional>
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
};

