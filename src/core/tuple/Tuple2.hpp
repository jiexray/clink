/**
 * A tuple with 2 fields.
 */
#pragma once
#include <memory>
#include <string>
#include "Tuple.hpp"
#include <sstream>

template <class T0, class T1>
class Tuple2 : public Tuple
{
public:
    std::shared_ptr<T0>         f0;
    std::shared_ptr<T1>         f1;

    Tuple2() {}

    Tuple2(std::shared_ptr<T0> value0, std::shared_ptr<T1> value1):
    f0(value0), f1(value1) {}

    ~Tuple2(){}

    int                         get_arity() {return 2;}

    const std::type_info&       get_field(int pos) {
        if (pos == 0) {
            return typeid(T0);
        } else if (pos == 1) {
            return typeid(T1);
        } else {
            throw std::invalid_argument("the position " + std::to_string(pos) + " is illegal in Tuple2");
        }
    }

    std::shared_ptr<void>       get_value(int pos) {
        switch (pos)
        {
        case 0:
            return f0;
        case 1:
            return f1;
        default:
            throw std::runtime_error("the position " + std::to_string(pos) + " is illegal in Tuple2");
        }
    }

    std::string                 to_string() {
        std::ostringstream oss;
        oss << "{" << *f0 << ": " << *f1 << "}";
        return oss.str();
        // return "Tuple2 {f0: " + IOUtils::to_string<T0>(f0) + ", f1: " + IOUtils::to_string<T1>(f1) + "}";
    }
};
