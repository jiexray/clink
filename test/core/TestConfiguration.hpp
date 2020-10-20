#pragma once

#include "Configuration.hpp"
#include "cxxtest/TestSuite.h"
#include <memory>
#include <iostream>

class TestConfiguration: public CxxTest::TestSuite
{
public:
    void testConfigurationSetValue( void ) {
        std::shared_ptr<Configuration> config = std::make_shared<Configuration>();

        config->set_value<std::string>("operator_id", std::make_shared<std::string>("operator_id_test"));
        std::shared_ptr<std::string> value = config->get_value<std::string>("operator_id");
        // std::cout << "get value: " << *value << std::endl;
        TS_ASSERT_EQUALS(*value, "operator_id_test");
    }
};

