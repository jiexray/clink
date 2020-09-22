/**
 * Util functions of Tuple.
 */
#pragma once
#include "Tuple2.hpp"

class TupleUtils
{
public:
    static std::shared_ptr<Tuple> create_tuple2_instance(std::shared_ptr<void> f0, std::shared_ptr<void> f1, 
                                            const std::type_info& f0_type, const std::type_info& f1_type) {
        if (f0_type == typeid(int)) {
            if (f1_type == typeid(int)) {
                return std::make_shared<Tuple2<int, int>>(std::static_pointer_cast<int>(f0), std::static_pointer_cast<int>(f1));
            } else if (f1_type == typeid(double)) {
                return std::make_shared<Tuple2<int, double>>(std::static_pointer_cast<int>(f0), std::static_pointer_cast<double>(f1));
            } else if (f1_type == typeid(std::string)) {
                return std::make_shared<Tuple2<int, std::string>>(std::static_pointer_cast<int>(f0), std::static_pointer_cast<std::string>(f1));
            }
        } else if (f0_type == typeid(double)) {
            if (f1_type == typeid(int)) {
                return std::make_shared<Tuple2<double, int>>(std::static_pointer_cast<double>(f0), std::static_pointer_cast<int>(f1));
            } else if (f1_type == typeid(double)) {
                return std::make_shared<Tuple2<double, double>>(std::static_pointer_cast<double>(f0), std::static_pointer_cast<double>(f1));
            } else if (f1_type == typeid(std::string)) {
                return std::make_shared<Tuple2<double, std::string>>(std::static_pointer_cast<double>(f0), std::static_pointer_cast<std::string>(f1));
            }
        } else if (f0_type == typeid(std::string)) {
            if (f1_type == typeid(int)) {
                return std::make_shared<Tuple2<std::string, int>>(std::static_pointer_cast<std::string>(f0), std::static_pointer_cast<int>(f1));
            } else if (f1_type == typeid(double)) {
                return std::make_shared<Tuple2<std::string, double>>(std::static_pointer_cast<std::string>(f0), std::static_pointer_cast<double>(f1));
            } else if (f1_type == typeid(std::string)) {
                return std::make_shared<Tuple2<std::string, std::string>>(std::static_pointer_cast<std::string>(f0), std::static_pointer_cast<std::string>(f1));
            }
        }
    }
};

