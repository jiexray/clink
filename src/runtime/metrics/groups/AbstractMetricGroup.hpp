/**
 * Abstract MetricGroup that contains key functionality for adding metrics and groups.
 */
#pragma once
#include "MetricGroup.hpp"
#include "MetricRegistry.hpp"
#include "TemplateHelper.hpp"
#include "ScopeFormat.hpp"
#include "SimpleCounter.hpp"
#include "LoggerFactory.hpp"
// #include "GenericMetricGroup.hpp"
#include "InheritableSharedPtr.hpp"
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <typeinfo>
#include <cstring>
#include <iostream>

enum ChildType{ 
    KEY, 
    VALUE,
    GENERIC
};

// class MetricRegistry;
// class GenericMetricGroup;

template <class A>
class AbstractMetricGroup: public MetricGroup, public inheritable_enable_shared_from_this<AbstractMetricGroup<A>>
{
private:
    typedef std::shared_ptr<Metric> MetricPtr;
    typedef std::shared_ptr<MetricRegistry> MetricRegistryPtr;
    typedef typename std::shared_ptr<A> ParentMetricGroupPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;

    static std::vector<std::string> make_scope_component(std::shared_ptr<MetricGroup> parent, std::string name) {
        std::vector<std::string> result;
        if (parent != nullptr) {
            std::vector<std::string>& parent_components = parent->get_scope_components();

            if (!parent_components.empty()) {
                for (int i = 0; i < parent_components.size(); i++) {
                    result.push_back(parent_components[i]);
                }
            }
        } 
        result.push_back(name);
        return result;
    }
protected:
    /* The parent group containing this group */
    ParentMetricGroupPtr                                    m_parent;
    
    /* The registry that this metric group belongs to */
    MetricRegistryPtr                                       m_registry;

    /* All metrics that are directly contained in this group */
    std::map<std::string, MetricPtr>                        m_metrics;
    std::mutex                                              m_global_mtx;

    /* All metric subgroups of this group */
    std::map<std::string, MetricGroupPtr>                   m_groups;

    /* The metrics scope represented by the group */
    std::vector<std::string>                                m_scope_components;

    /* Array containing the metrics scope represented by this group for each reporter, lazy calculate */
    std::string*                                            m_scope_strings;
    bool*                                                   m_scope_strings_init;
    int                                                     m_scope_strings_size;

    /* The logical metrics scope represented by this group for each reporter, lazy calculate */
    std::string*                                            m_logical_scope_strings;
    bool*                                                   m_logical_scope_strings_init;
    int                                                     m_logical_scope_strings_size;

    static std::shared_ptr<spdlog::logger>                  m_logger;
    bool                                                    m_closed;



    virtual void                                            put_variables(std::map<std::string, std::string>& variables) {}

public:
    AbstractMetricGroup(std::shared_ptr<MetricRegistry> registry, ParentMetricGroupPtr parent, std::string name) {
        this->m_registry = registry;

        m_scope_components = make_scope_component(parent, name);

        this->m_parent = parent;
        
        m_scope_strings_size = registry->get_number_reporters();
        m_scope_strings = new std::string[m_scope_strings_size];
        m_scope_strings_init = new bool[m_scope_strings_size];
        
        m_logical_scope_strings_size = registry->get_number_reporters();
        m_logical_scope_strings = new std::string[m_logical_scope_strings_size];
        m_logical_scope_strings_init = new bool[m_logical_scope_strings_size];

        memset(m_scope_strings_init, 0, m_scope_strings_size);
        memset(m_logical_scope_strings_init, 0, m_logical_scope_strings_size);

        m_closed = false;
    }

    AbstractMetricGroup(MetricRegistryPtr registry, const std::vector<std::string>& scope, ParentMetricGroupPtr parent) {
        this->m_registry = registry;

        for (int i = 0; i < scope.size(); i++) {
            m_scope_components.push_back(scope[i]);
        }

        this->m_parent = parent;
        
        m_scope_strings_size = registry->get_number_reporters();
        m_scope_strings = new std::string[m_scope_strings_size];
        m_scope_strings_init = new bool[m_scope_strings_size];
        
        m_logical_scope_strings_size = registry->get_number_reporters();
        m_logical_scope_strings = new std::string[m_logical_scope_strings_size];
        m_logical_scope_strings_init = new bool[m_logical_scope_strings_size];

        memset(m_scope_strings_init, 0, m_scope_strings_size);
        memset(m_logical_scope_strings_init, 0, m_logical_scope_strings_size);

        m_closed = false;
    }

    ~AbstractMetricGroup() {
        delete[] m_scope_strings;
        delete[] m_scope_strings_init;
        delete[] m_logical_scope_strings;
        delete[] m_logical_scope_strings_init;
    }

    // ----------------------------------------------------------------------------- 
    // Operator for logical scope 
    // ----------------------------------------------------------------------------- 
    std::string get_logical_scope(char delimiter, int reporter_index) {
        if (m_logical_scope_strings_size == 0 || (reporter_index < 0 || reporter_index >= m_logical_scope_strings_size)) {
            return create_logical_scope(delimiter);
        } else {
            if (m_logical_scope_strings_init[reporter_index] == false) {
                m_logical_scope_strings_init[reporter_index] = true;
                m_logical_scope_strings[reporter_index] = create_logical_scope(delimiter);
            }
            return m_logical_scope_strings[reporter_index];
        }
    }

    std::string create_logical_scope(char delimiter) {
        std::string group_name = get_group_name();
        return m_parent == nullptr ? group_name : m_parent->get_logical_scope(delimiter) + std::to_string(delimiter) + group_name;
    }

    virtual std::string get_group_name() {return "abstract-group";}

    // ----------------------------------------------------------------------------- 
    // Operator for scope components
    // ----------------------------------------------------------------------------- 
    std::vector<std::string>& get_scope_components() override {
        return m_scope_components;
    }

    /* Return the fully qualified metric name */
    std::string get_metric_identifier(const std::string& metric_name) override {
        return get_metric_identifier(metric_name, -1, m_registry->get_delimiter());
    }

    std::string get_metric_identifier(const std::string& metric_name, int report_index, char delimiter) {
        if (m_scope_strings_size == 0 || (report_index < 0 || report_index >= m_scope_strings_size)) {
            std::string new_scope_string = ScopeFormat::concat(delimiter, m_scope_components);
            return new_scope_string + delimiter + metric_name;
        } else {
            if (m_scope_strings_init[report_index] == false) {
                m_scope_strings_init[report_index] = true;
                m_scope_strings[report_index] = ScopeFormat::concat(delimiter, m_scope_components);
            }

            return m_scope_strings[report_index] + delimiter + metric_name;
        }
    }

    // ----------------------------------------------------------------------------- 
    //  Closing
    // ----------------------------------------------------------------------------- 

    virtual void close() {
        // Note: base class do not hold lock, the entry of close hold the lock.
        if (!m_closed) {
            m_closed = true;

            auto groups_it = m_groups.begin();
            while (groups_it != m_groups.end())
            {
                groups_it->second->close();
            }
            m_groups.clear();

            auto it = m_metrics.begin();
            while (it != m_metrics.end()) {
                m_registry->unregister_metric(it->second, it->first, this->shared_from_this());
            }
            m_metrics.clear();
            
        }
    }

    bool is_closed() {
        return m_closed;
    }

    // ----------------------------------------------------------------------------- 
    //  Metrics
    // ----------------------------------------------------------------------------- 
    std::shared_ptr<Counter> counter(const std::string& name) {
        return counter(name, std::make_shared<SimpleCounter>());
    }

    /* Register a Counter */
    std::shared_ptr<Counter> counter(const std::string& name, std::shared_ptr<Counter> counter) {
        add_metric(name, counter);
        return counter;
    }

    /* Register a Meter */
    std::shared_ptr<Meter> meter(const std::string& name, std::shared_ptr<Meter> meter) {
        add_metric(name, meter);
        return meter;
    }

    /* Register a Gauge */
    std::shared_ptr<Gauge> gauge(const std::string& name, std::shared_ptr<Gauge> gauge) {
        add_metric(name, gauge);
        return gauge;
    }


    void add_metric(const std::string& name, std::shared_ptr<Metric> metric) {
        std::unique_lock<std::mutex> register_lck(m_global_mtx);
        if (m_closed) {
            return;
        }
        

        if (m_metrics.find(name) != m_metrics.end()) {
            SPDLOG_LOGGER_WARN(m_logger, "name collision: Group already contains a Metric with the name {}", name);
            return;
        }

        m_metrics.insert(std::make_pair(name, metric));
        m_registry->register_metric(metric, name, this->shared_from_this());
    }

    // ----------------------------------------------------------------------------- 
    // Groups
    // ----------------------------------------------------------------------------- 

    // Creates a new MetricGroup and adds it to this group sub-groups
    std::shared_ptr<MetricGroup> add_group(const std::string& name) {
        return add_group(name, ChildType::GENERIC);
    }

    std::shared_ptr<MetricGroup> add_group(const std::string& name, ChildType child_type) {
        std::unique_lock<std::mutex> register_lck(m_global_mtx);
        if (m_closed) {
            return nullptr;
        }

        if (m_groups.find(name) != m_groups.end()) {
            return m_groups[name];
        } else {
            std::shared_ptr<AbstractMetricGroup<MetricGroup>> new_group = 
                            std::make_shared<AbstractMetricGroup<MetricGroup>>(m_registry, this->shared_from_this(), name);
            m_groups.insert(std::make_pair(name, new_group));
            return new_group;
        }
    }
};

template <class A>
std::shared_ptr<spdlog::logger> AbstractMetricGroup<A>::m_logger = LoggerFactory::get_logger(typeid(AbstractMetricGroup<A>).name());