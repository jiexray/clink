/**
 * A executable bound to a specfic operator in the chain, such that it can be picked for downstream mailbox.
 */
#pragma once

#include "Runnable.hpp"
#include <memory>
#include <functional>
#include "string"

class Mail
{
public:
    typedef std::function<void()> MailFunc;
private:
    // std::shared_ptr<Runnable>           m_runnable;
    MailFunc                            m_mail_func;

    std::string                         m_description;

public:
    Mail(MailFunc mail_func, const std::string & description) :
        m_mail_func(mail_func), m_description(description) {
    }

    std::string                         to_string() {return m_description;}

    void                                run() {m_mail_func();}

};