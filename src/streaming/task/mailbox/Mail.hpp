/**
 * A executable bound to a specfic operator in the chain, such that it can be picked for downstream mailbox.
 */
#pragma once

#include "Runnable.hpp"
#include <memory>
#include "string"

class Mail
{
private:
    std::shared_ptr<Runnable>           m_runnable;

    std::string                         m_description;

public:
    Mail(std::shared_ptr<Runnable> runnable, std::string description):
    m_runnable(runnable), m_description(description) {}

    std::string                         to_string() {return m_description;}

    void                                run() {m_runnable->run();}

};