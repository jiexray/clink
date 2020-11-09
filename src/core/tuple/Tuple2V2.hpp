/**
 * A tuple with 2 fields.
 */
#pragma once
#include <memory>
#include <string>
#include "Tuple.hpp"
#include "TemplateHelper.hpp"
#include <sstream>
#include <cstring>

template <class T0, class T1, int SIZE0, int SIZE1> class Tuple2V2;

template <class T0, class T1, int SIZE0 = sizeof(T0), int SIZE1 = sizeof(T1)>
class Tuple2V2 : public Tuple {
public:
    T0 f0;
    T1 f1;

    Tuple2V2() {}
    Tuple2V2(const T0* value0, const T1* value1):
    f0(*value0), f1(*value1){}

    std::string                 to_string() {
        std::ostringstream oss;
        oss << "{" << f0 << ": " << f1 << "}";
        return oss.str();
        // return "Tuple2 {f0: " + IOUtils::to_string<T0>(f0) + ", f1: " + IOUtils::to_string<T1>(f1) + "}";
    }

    int                         get_arity() {return 2;}

    int                         get_buf_size() override {
        return sizeof(Tuple2V2<T0, T1>);
    }
};

template<class T0, int SIZE0, int SIZE1>
class Tuple2V2<T0, const char*, SIZE0, SIZE1>: public Tuple {
public:
    T0 f0;
    char f1[SIZE1];

    Tuple2V2() {}
    Tuple2V2(const T0* value0, const char* value1): f0(*value0){
        if (strlen(value1) >= SIZE1) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(strlen(value1)) 
            + " >= " + std::to_string(SIZE1));
        }
        strncpy(f1, value1, SIZE1);
    }

    int                         get_arity() {return 2;}

    std::string                 to_string() {
        std::ostringstream oss;
        oss << "{" << f0 << ": " << std::string(f1) << "}";
        return oss.str();
    }

    int                         get_buf_size() override {
        return sizeof(Tuple2V2<T0, const char*, SIZE0, SIZE1>);
    }
};

template<class T1, int SIZE0, int SIZE1>
class Tuple2V2<const char*, T1, SIZE0, SIZE1>: public Tuple {
public:
    char f0[SIZE0];
    T1 f1;

    Tuple2V2() {}
    Tuple2V2(const char* value0, const T1* value1): f1(*value1){
        if (strlen(value0) >= SIZE0) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(strlen(value0)) 
            + " >= " + std::to_string(SIZE0));
        }
        strncpy(f0, value0, SIZE0);
    }

    int                         get_arity() {return 2;}

    std::string                 to_string() {
        std::ostringstream oss;
        oss << "{" << std::string(f0) << ": " << f1 << "}";
        return oss.str();
    }

    int                         get_buf_size() override {
        return sizeof(Tuple2V2<const char*, T1, SIZE0, SIZE1>);
    }
};

template<int SIZE0, int SIZE1>
class Tuple2V2<const char*, const char*, SIZE0, SIZE1>: public Tuple {
public:
    char f0[SIZE0];
    char f1[SIZE1];

    Tuple2V2() {}
    Tuple2V2(const char* value0, const char* value1){
        if (strlen(value0) >= SIZE0) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(strlen(value0)) 
            + " >= " + std::to_string(SIZE0));
        }
        strncpy(f0, value0, SIZE0);

        if (strlen(value1) >= SIZE1) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(strlen(value1)) 
            + " >= " + std::to_string(SIZE1));
        }
        strncpy(f1, value1, SIZE1);
    }

    int                         get_arity() {return 2;}

    std::string                 to_string() {
        std::ostringstream oss;
        oss << "{" << std::string(f0) << ": " << std::string(f1) << "}";
        return oss.str();
    }

    int                         get_buf_size() override {
        return sizeof(Tuple2V2<const char*, const char*, SIZE0, SIZE1>);
    }
};