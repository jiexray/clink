/**
 * Helper class/functions for templates.
 */
#pragma once
#include <type_traits>
#include <memory>

class NullType {};
struct EmptyType {};

template <int v>
struct Int2Type {
    enum {value = v};
};

template <typename T>
struct Type2Type {
    typedef T OriginalType;
};


template <typename U> struct IsNullType: std::false_type {};
template <> struct IsNullType<NullType>: std::true_type {};

template<typename T> struct IsSharedPtr : std::false_type {};
template<typename T> struct IsSharedPtr<std::shared_ptr<T>> : std::true_type {};

template <class T>
class TemplateHelper {
public:
    enum { is_null_type     = IsNullType<T>::value,
           is_shared_ptr    = IsSharedPtr<T>::value};
};


namespace TemplateHelperUtil{
    template<bool b, typename T, typename U>
    struct select
    {
        typedef T type;
    };
    template<typename T, typename U>
    struct select<false, T, U>
    {
        typedef U type;
    };

    template <typename K>
    struct ParamOptimize {
        typedef typename select<std::is_fundamental<K>::value || std::is_pointer<K>::value || std::is_reference<K>::value, K, K&>::type type;
        typedef typename select<std::is_fundamental<K>::value || std::is_pointer<K>::value || std::is_reference<K>::value, K, K const&>::type const_type;
    };
};
