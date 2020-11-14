#pragma once
#include "AbstractID.hpp"
#include "KvStateID.hpp"
#include "cxxtest/TestSuite.h"
#include <memory>
#include <iostream>

class TestAbstractID: public CxxTest::TestSuite
{
public:
    void testAbstractId() {
        srand(time(0));
        std::cout << "test testAbstractId()" << std::endl;
        AbstractID id1;
        AbstractID id2;

        std::cout << "id1: " << id1.to_string() << std::endl;
        std::cout << "id2: " << id2.to_string() << std::endl;

        std::cout << "id1 compare to id2: " << (id1 < id2 ? "id1 < id2" : "id >= id2") << std::endl;
        std::cout << "id1 == id2: " << (id1 == id2? "id1 == id2": "id1 != id2") << std::endl;
    }

    void testKvStateID( void ) {
        srand(time(0));
        std::cout << "test testKvStateID()" << std::endl;

        KvStateID id1;
        KvStateID id2;

        std::cout << "id1: " << id1.to_string() << std::endl;
        std::cout << "id2: " << id2.to_string() << std::endl;

        std::cout << "id1 compare to id2: " << (id1 < id2 ? "id1 < id2" : "id >= id2") << std::endl;
        std::cout << "id1 == id2: " << (id1 == id2? "id1 == id2": "id1 != id2") << std::endl;
    }

    void testStateCopy( void ) {
        std::cout << "test testStateCopy()" << std::endl;
        srand(time(0));

        KvStateID id1;
        KvStateID id2 = id1;

        std::cout << "id1: " << id1.to_string() << std::endl;
        std::cout << "id2: " << id2.to_string() << std::endl;

        std::cout << "id1 compare to id2: " << (id1 < id2 ? "id1 < id2" : "id >= id2") << std::endl;
        std::cout << "id1 == id2: " << (id1 == id2? "id1 == id2": "id1 != id2") << std::endl;

        TS_ASSERT_EQUALS(&id1 != &id2 , true);
    }
};