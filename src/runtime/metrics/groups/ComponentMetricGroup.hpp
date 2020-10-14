/**
 * Abstract MetricGroup for system components (e.g., TaskManager, Job, Task, Operator).
 */
#pragma once
#include "AbstractMetricGroup.hpp"
#include <map>
#include <vector>

template <class P>
class ComponentMetricGroup : public AbstractMetricGroup<P>
{
private:
    
public:
    ComponentMetricGroup(std::shared_ptr<MetricRegistry> registry, const std::vector<std::string>& scope, std::shared_ptr<P> parent): 
                        AbstractMetricGroup<P>(registry, scope, parent){}

    virtual void close() {
        AbstractMetricGroup<P>::close();
    }
};

