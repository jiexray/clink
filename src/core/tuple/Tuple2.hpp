/**
 * A tuple with 2 fields.
 */
#pragma once
#include <memory>
#include <string>
#include "Tuple.hpp"


template <class T1, class T2>
class Tuple2 : public Tuple
{
public:
    std::shared_ptr<T1>         f0;
    std::shared_ptr<T2>         f1;

    Tuple2() {}

    Tuple2(std::shared_ptr<T1> value0, std::shared_ptr<T2> value1):
    f0(value0), f1(value1) {}

    ~Tuple2(){}

    int                         get_arity() {return 2;}

    const std::type_info&       get_field(int pos) {
        if (pos == 0) {
            return typeid(T1);
        } else if (pos == 1) {
            return typeid(T2);
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
        return "Tuple2";
    }
};
