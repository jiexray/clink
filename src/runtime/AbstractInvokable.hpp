/**
 * This is the abstract class for every task that can be executed by a TaskManager.
 */
#pragma once

#include "Environment.hpp"
#include "Configuration.hpp"
#include <memory>

class AbstractInvokable
{
private:
    std::shared_ptr<Environment>        m_environment;
public:
    AbstractInvokable() = default;
    AbstractInvokable(std::shared_ptr<Environment> environment): m_environment(environment) {}

    /* Properties */
    std::shared_ptr<Environment>        get_environment() {return m_environment;}

    /* Core methods */
    virtual void                        invoke() = 0;
    virtual void                        cancel() = 0;
};
