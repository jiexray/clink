/**
 * A simple low-overhead Counter that is not thread-safe
 */
#pragma once
#include "Counter.hpp"

class SimpleCounter: public Counter
{
private:
    long    m_count;
public:
    SimpleCounter() {m_count = 0;}

    void inc()          {m_count++;}
    void inc(long n)    {m_count += n;}
    void dec()          {m_count--;}
    void dec(long n)    {m_count -= n;}
    virtual long get_count()    {return m_count;}
};
