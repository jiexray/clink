#pragma once
#include "MeterView.hpp"
#include "cxxtest/TestSuite.h"

class TestMeter : public CxxTest::TestSuite
{    
public:
    void testMeterViewCreate( void ) {
        std::shared_ptr<MeterView> meter_view = std::make_shared<MeterView>(10);
    }
};

