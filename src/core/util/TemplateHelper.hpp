/**
 * Helper class/functions for templates.
 */
#pragma once

class NullType {};
struct EmptyType {};

template <int v>
struct Int2Type {
    enum {value = v};
};

template <typename U> struct IsNullType {
    enum {result = false};
};
template <> struct IsNullType<NullType> {
    enum {result = true};    
};

template <class T>
class TemplateHelper {
public:
    enum { is_null_type = IsNullType<T>::result };
};