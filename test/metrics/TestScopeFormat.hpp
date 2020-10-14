#pragma once
#include "ScopeFormat.hpp"
#include "ScopeFormats.hpp"
#include "cxxtest/TestSuite.h"

class TestScopeFormat: public CxxTest::TestSuite
{
public:
    void testScopeFormatsCreate() {
        ScopeFormats* scope_formats = ScopeFormats::get_instance();
        std::shared_ptr<TaskManagerScopeFormat> task_manager_scope_format = scope_formats->get_task_manager_format();
        std::vector<std::string> scope = task_manager_scope_format->format_scope("host-1", "tm-1");
        TS_ASSERT_EQUALS(scope.size(), 3);
        TS_ASSERT_EQUALS(scope[0], "host-1");
        TS_ASSERT_EQUALS(scope[1], "taskmanager");
        TS_ASSERT_EQUALS(scope[2], "tm-1");
    }
};