/**
 * Util class for collection operations.
 */
#pragma once
#include "TemplateHelper.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>

class CollectionUtils
{
public:
    template<class K, class V>
    static std::vector<V> map_to_values(std::map<K, V> m) {
        typedef typename std::map<K, V> MapType;
        typedef typename MapType::iterator MapIter;
        typedef typename std::vector<V> VectorType;

        VectorType vec;
        MapIter it = m.begin();

        while (it != m.end()) {
            vec.push_back(it->second);
            ++it;
        }
        return vec;
    }

    template<class B, class D>
    static std::vector<std::shared_ptr<B>> convert_vector_inner_type_to_base(const std::vector<std::shared_ptr<D>>& o) {
        if (!std::is_base_of<B, D>::value) {
            throw std::invalid_argument("ERROR!, cannot convert class from " + std::string(typeid(B).name()) + " to " + typeid(D).name());
        }
        std::vector<std::shared_ptr<B>> vec;
        for (int i = 0; i < o.size(); i++){
            vec.push_back(o[i]);
        }
        return vec;
    }
};

