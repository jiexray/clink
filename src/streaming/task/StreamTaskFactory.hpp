/**
 * A object factory for StreamTask.
 */
#pragma once

#include "StreamTask.hpp"
#include "OneInputStreamTask.hpp"
#include "SourceStreamTask.hpp"
#include "TemplateHelper.hpp"
#include <map>
#include <memory>
#include <string>

namespace StreamTaskFactoryCreator {

    //----------------------------------
    // Creator for OneInputStreamTask
    //----------------------------------
    template<class IN, class OUT = NullType>
    std::shared_ptr<AbstractInvokable> create_one_input_stream_task(std::shared_ptr<Environment> env) {
        return std::make_shared<OneInputStreamTask<IN, OUT>>(env);
    }

    //----------------------------------
    // Creator for SourceStreamTask
    //----------------------------------
    template<class OUT>
    std::shared_ptr<AbstractInvokable> create_source_stream_task(std::shared_ptr<Environment> env) {
        return std::make_shared<SourceStreamTask<OUT>>(env);
    }
};

template <class AbstractStreamTask = AbstractInvokable, typename IdentifierType = std::string, typename StreamTaskCreator = std::shared_ptr<AbstractStreamTask> (*)(std::shared_ptr<Environment>)>
class StreamTaskFactory
{
public:
    static StreamTaskFactory* instance() {
        if (!m_instance_ptr) {
            static StreamTaskFactory the_instance;
            m_instance_ptr = &the_instance;
        } 
        return m_instance_ptr;
    }

    bool                                register_stream_task(const IdentifierType& id, StreamTaskCreator creator) {
        // return m_associations.insert(AssocMap::value_type(id, creator)).second;
        return m_associations.insert(std::make_pair(id, creator)).second;
    }

    bool                                unregister_stream_task(const IdentifierType& id) {
        return m_associations.erase(id) == 1;
    }              

    std::shared_ptr<AbstractStreamTask> create_task(const IdentifierType& id, std::shared_ptr<Environment> env) {
        typename AssocMap::const_iterator it = m_associations.find(id);
        if (it != m_associations.end()) {
            return (it->second)(env);
        }
        throw std::runtime_error("Unknown identifier");
    }
private:
    StreamTaskFactory() {}
    StreamTaskFactory(const StreamTaskFactory& stream_task_factory){}
    ~StreamTaskFactory() {
        m_instance_ptr = nullptr;
    }

    static StreamTaskFactory*                               m_instance_ptr;

    typedef std::map<IdentifierType, StreamTaskCreator>     AssocMap;
    AssocMap                                                m_associations;
};

template <class AbstractStreamTask, typename IdentifierType, typename StreamTaskCreator>
StreamTaskFactory<AbstractStreamTask, IdentifierType, StreamTaskCreator>* StreamTaskFactory<AbstractStreamTask, IdentifierType, StreamTaskCreator>::m_instance_ptr = 0;

