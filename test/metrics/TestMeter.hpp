#pragma once
#include "MeterView.hpp"
#include "Gauge.hpp"
#include "cxxtest/TestSuite.h"

class TestMeter : public CxxTest::TestSuite
{    
public:
    void testMeterViewCreate( void ) {
        std::shared_ptr<MeterView> meter_view = std::make_shared<MeterView>(10);
    }

    void testGaugeCreate( void ) {
        std::shared_ptr<Gauge> test_gauge_int = std::make_shared<TestGaugeInt>();
        std::shared_ptr<Gauge> test_gauge_double = std::make_shared<TestGaugeDouble>();

        int v;
        double v_double;
        std::string v_string;
        test_gauge_int->get_value(&v);
        test_gauge_double->get_value(&v_double);
        test_gauge_int->get_value(v_string);
        std::cout << "int value: " << v  << " should equal to " << 112 << std::endl;
        std::cout << "double value: " << v_double << " should equal to " << 112.01 << std::endl;
        std::cout << "int value to_string: " << v << " should equal to " << 112 << std::endl;
    }
};

