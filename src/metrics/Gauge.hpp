/**
 * A Guage is a Metric that calculates a specific value at a point in time.
 */
#pragma once
#include "Metric.hpp"
#include "TemplateHelper.hpp"
#include <string>

struct GaugeVisitor
{
    virtual void operator() (int*) const = 0;
    virtual void operator() (double*) const = 0;
    virtual void operator() (std::string&) const = 0;
};

template <typename GaugeType>
struct GaugeVisitorImpl: GaugeVisitor {
    GaugeType& m_data;
    GaugeVisitorImpl(GaugeType& data): m_data(data) {}
    virtual void operator() (int* v) const {*v = m_data.get_value_impl();}
    virtual void operator() (double* v) const {*v = m_data.get_value_impl();}
    virtual void operator() (std::string& s) const {s = std::to_string(m_data.get_value_impl());}
};


class Gauge: public Metric
{
protected:
    Gauge(GaugeVisitor& visitor): get_value(visitor) {}
public:
    GaugeVisitor& get_value;
    virtual ~Gauge() {delete &get_value;}
};

class TestGaugeInt: public Gauge {
private:
    int get_value_impl() const {
        return 112;
    }
public:
    friend struct GaugeVisitorImpl<TestGaugeInt>;
    TestGaugeInt(): Gauge(*(new GaugeVisitorImpl<TestGaugeInt>(*this))){}
    virtual ~TestGaugeInt() {}
};

class TestGaugeDouble: public Gauge {
private:
    double get_value_impl() const {
        return 112.01;
    }
public:
    friend struct GaugeVisitorImpl<TestGaugeDouble>;
    TestGaugeDouble(): Gauge(*(new GaugeVisitorImpl<TestGaugeDouble>(*this))){}
    virtual ~TestGaugeDouble() {}
};