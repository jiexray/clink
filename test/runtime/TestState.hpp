#pragma once

#include "cxxtest/TestSuite.h"
#include "NestedStateMap.hpp"
#include "NestedMapsStateTable.hpp"
#include "InternalKeyContextImpl.hpp"
#include <iostream>
#include <string>

struct MyState{
    int f0;
    int f1;
    MyState() {}
    MyState(const MyState & state) {
        f0 = state.f0;
        f1 = state.f1;
    }
    MyState(int ff0, int ff1): f0(ff0), f1(ff1){}
};

class TestState: public CxxTest::TestSuite
{
public:
    void testStateMap() {
        std::cout << "test testStateMap()" << std::endl;
        NestedStateMap<int, std::string, int> state_map;
        state_map.put(10, "ns-1", 199);

        int state_1 = state_map.get(10, "ns-1");
        TS_ASSERT_EQUALS(state_1, 199);

        NestedStateMap<int, std::string, MyState> state_map_2;
        state_map_2.put(10, "ns_1", MyState(100, 101));
        state_map_2.put(10, "ns_1", MyState(10, 11));

        MyState state_2 = state_map_2.get(10, "ns_1");
        TS_ASSERT_EQUALS(state_2.f0, 10);
        TS_ASSERT_EQUALS(state_2.f1, 11);
    }

    void testStateTable() {
        std::cout << "test testStateTable()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, int> state_table(key_context);

        key_context->set_current_key(101);

        state_table.put("ns-1", 101);
        state_table.put("ns-1", 102);

        int state_1 = state_table.get("ns-1");
        TS_ASSERT_EQUALS(state_1, 102);
    }

    
};

