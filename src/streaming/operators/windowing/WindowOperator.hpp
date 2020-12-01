#pragma once

#include "AbstractUdfStreamOperator.hpp"
#include "InternalWindowFunction.hpp"
#include "OneInputStreamOperator.hpp"
#include "Triggerable.hpp"
#include "WindowAssigner.hpp"
#include "Trigger.hpp"
#include "StateDescriptor.hpp"
#include "InternalKvState.hpp"
#include "InternalListState.hpp"
#include "DefaultKeyedStateStore.hpp"
#include "InternalTimerService.hpp"
#include "StreamOperatorStateHandler.hpp"
#include "HeapKeyedStateBackend.hpp"
#include "TimestampedCollector.hpp"
#include "InternalTimerServiceImpl.hpp"
#include "InternalTimeServiceManager.hpp"
#include "TemplateHelper.hpp"
#include <string>
#include <memory>

/**
  Regular per-window state store for use with ProcessWindowFunction.
 */
template <class K, class IN, class W>
class PerWindowStateStore : public DefaultKeyedListStateStore<K, W, IN>{
private:
    typedef KeyedStateBackend<K, W, std::vector<IN>, ListState<IN>, InternalListState<K, W, IN>> KeyedListStateBackendType;
    const W* _window;
public:
    PerWindowStateStore(KeyedListStateBackendType& keyed_state_backend, ExecutionConfig& execution_config):
        DefaultKeyedListStateStore<K, W, IN>(keyed_state_backend, execution_config) {}

    ListState<IN>& get_partitioned_state(const ListStateDescriptor<IN>& state_descriptor) override{
        return this->m_keyed_state_backend.get_partitioned_state(*_window, state_descriptor);
    }

    void set_window(const W& window) {
        _window = &window;
    }
};

/**
  A utility class for handling ProcessWindowFunction invocations. This can be reused by 
  setting the key and window fields.
 */
template <class K, class IN, class W>
class WindowOperatorWindowContext : public InternalWindowFunctionContext {
private:
    const W* _window;

    PerWindowStateStore<K, IN, W>* m_window_state;

    typedef KeyedStateBackend<K, W, std::vector<IN>, ListState<IN>, InternalListState<K, W, IN>> KeyedListStateBackendType;

    InternalTimerService<W>& m_internal_timer_service;
public:
    WindowOperatorWindowContext(
            W* window, 
            KeyedListStateBackendType& state_backend, 
            ExecutionConfig& execution_config,
            InternalTimerService<W>& internal_timer_service): 
            _window(window),
            m_internal_timer_service(internal_timer_service) {
        m_window_state = new PerWindowStateStore<K, IN, W>(state_backend, execution_config);
    }

    ~WindowOperatorWindowContext() {
        delete m_window_state;
    }

    void set_window(const W& window) {
        this->_window = &window;
    }

    long current_process_time() override {
        return m_internal_timer_service.current_processing_time();
    }

    long current_watermark() override {
        return m_internal_timer_service.current_watermark();
    }

    KeyedMapStateStore& window_map_state() override {
        throw std::runtime_error("Not implemented");
    }

    KeyedListStateStore& window_list_state() override {
        m_window_state->set_window(*_window);
        return *m_window_state;
    }

    KeyedMapStateStore& global_map_state() override {
        throw std::runtime_error("Not implemented");
    }

    KeyedListStateStore& global_list_state() override{
        // TODO: implement global state.
    }
};

template <class N>
class WindowOperatorWindowAssignerContext: public WindowAssignerContext {
private:
    InternalTimerService<N>& m_internal_timer_service;
public:
    WindowOperatorWindowAssignerContext(InternalTimerService<N>& internal_timer_service): 
            m_internal_timer_service(internal_timer_service) {

    }
    long get_current_processing_time() override {
        return m_internal_timer_service.current_processing_time();
    }
};

template <class K, class IN, class W>
class WindowOperatorContext: public TriggerContext {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;

    K m_key;
    const W* _window; // no need to free
    Trigger<IN, W>& m_trigger;

    std::vector<W> merged_windows;

    InternalTimerService<W>& m_internal_timer_service;
public:

    WindowOperatorContext(
            ConstParamK key, 
            W* window,
            InternalTimerService<W>& internal_timer_service,
            Trigger<IN, W>& trigger): 
            m_key(key), 
            _window(window),
            m_internal_timer_service(internal_timer_service),
            m_trigger(trigger) {
    }

    void set_window(const W& window) {
        this->_window = &window;
    }

    const W& get_window() {
        return *_window;
    }

    void set_key(ConstParamK key) {
        this->m_key = key;
    }

    ConstParamK get_key() {
        return m_key;
    }

    long get_current_processing_time() override {
        return m_internal_timer_service.current_processing_time();
    }

    long get_current_watermark() override {
        return m_internal_timer_service.current_watermark();
    }

    void register_processing_time_timer(long time) override {
        std::cout << "TriggerContext::register_processing_time_timer() window: " << this->_window->to_string() << ", trigger timestamp:" << time << std::endl;
        assert(this->_window != nullptr);
        m_internal_timer_service.register_processing_time_timer(*(this->_window), time);
    }

    void register_event_time_timer(long time) override {
        assert(this->_window != nullptr);
        m_internal_timer_service.register_event_time_timer(*(this->_window), time);
    }

    void delete_processing_time_timer(long time) override {
        assert(this->_window != nullptr);
        m_internal_timer_service.delete_processing_time_timer(*(this->_window), time);
    }

    void delete_event_time_timer(long time) override {
        assert(this->_window != nullptr);
        m_internal_timer_service.delete_event_time_timer(*(this->_window), time);
    }

    TriggerResult on_element(StreamRecordV2<IN>* element) {
        return m_trigger.on_element(&element->val, element->timestamp, *(this->_window), *this);
    }

    TriggerResult on_processing_time(long time) {
        return m_trigger.on_processing_time(time, *(this->_window), *this);
    }

    TriggerResult on_event_time(long time) {
        return m_trigger.on_event_time(time, *(this->_window), *this);
    }

    TriggerResult clear() {
        assert(this->_window != nullptr);
        m_trigger.clear(*(this->_window), *this);
    }

    std::string to_string() {
        return "WindowOperatorContext{key=" + 
            StringUtils::to_string<K>(m_key) + 
            ", window=" + this->_window->to_string() + "}";
    }
};

/**
  An operator that implements the logic for windowing based on a WindowAssigner and Trigger.

  @param <K> The type of key returned by the KeySelector
  @param <IN> The type of the incoming elements.
  @param <OUT> The type of the elements emitted by the InternalWindowFunction.
  @param <W> The type of Window that WindowAssigner assigns.
 */
template<class K, class IN, class ACC, class OUT, class W>
class WindowOperator: 
    public AbstractUdfStreamOperator<InternalWindowFunction<ACC, OUT, K, W>, OUT>,
    public OneInputStreamOperator<IN, OUT>,
    public Triggerable<K, W>,
    public KeyContext<K> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::type ParamACC;
    typedef typename TemplateHelperUtil::ParamOptimize<ACC>::const_type ConstParamACC;

    typedef HeapKeyedStateBackend<K, W, std::vector<IN>, ListState<IN>, InternalListState<K, W, IN>> HeapKeyedStateBackendType;
    typedef AbstractKeyedStateBackend<K, W, std::vector<IN>, ListState<IN>, InternalListState<K, W, IN>> AbstractKeyedStateBackendType;
    typedef StreamOperatorStateHandlerImpl<K, W, std::vector<IN>, ListState<IN>, InternalListState<K, W, IN>> StreamOperatorStateHandlerType;

    // -------------------------------------------------------------
    // Configuration values and user functions
    // -------------------------------------------------------------
    WindowAssigner<IN, W>& m_window_assigner;

    Trigger<IN, W>& m_trigger;

    const StateDescriptor<ListState<IN>, std::vector<int>>& m_window_state_descriptor;

    // -------------------------------------------------------------
    // State that is not checkpointed
    // -------------------------------------------------------------

    /** The state in which the window contexts is stored. Each window is a name space */
    InternalListState<K, W, IN>& m_window_state;

    std::shared_ptr<TimestampedCollector<OUT>> m_timestamped_collector;

    WindowOperatorContext<K, IN, W>* m_trigger_context;

    WindowOperatorWindowContext<K, IN, W>* m_process_context;

    WindowAssignerContext* m_window_assigner_context;

    // -------------------------------------------------------------
    // Ought to be placed in AbstractStreamOperator
    // -------------------------------------------------------------
    StreamOperatorStateHandlerType* m_state_handler;

    InternalTimeServiceManager<K, W>* m_time_service_manager;

    // -------------------------------------------------------------
    // State that needs to be checkpointed
    // -------------------------------------------------------------
    InternalTimerService<W>* _internal_timer_service;

    // -------------------------------------------------------------
    // Time handler
    // -------------------------------------------------------------
    ProcessingTimeService& m_processing_time_service;

public:

    WindowOperator(
            WindowAssigner<IN, W>& window_assigner,
            std::shared_ptr<InternalWindowFunction<ACC, OUT, K, W>> window_function,
            Trigger<IN, W>& trigger,
            const StateDescriptor<ListState<IN>, std::vector<int>>& window_state_descriptor,
            HeapKeyedStateBackendType& keyed_state_backend,
            ExecutionConfig& execution_config,
            ProcessingTimeService& processing_time_service):
            AbstractUdfStreamOperator<InternalWindowFunction<ACC, OUT, K, W>, OUT>(window_function),
            m_state_handler(new StreamOperatorStateHandlerType(keyed_state_backend, execution_config)),
            m_window_assigner(window_assigner),
            m_trigger(trigger),
            m_window_state_descriptor(window_state_descriptor),
            m_window_state(keyed_state_backend.get_or_create_internal_keyed_state(window_state_descriptor)),
            m_processing_time_service(processing_time_service)
            {
        _internal_timer_service = nullptr;
        // initialiize InternalTimeServiceManager
        m_time_service_manager = new InternalTimeServiceManager<K, W>(
                keyed_state_backend.get_key_group_range(),    // key_group_range
                *this,    // key-context, the operator
                m_processing_time_service);    // ProcessingTimeService
    }

    ~WindowOperator() {
        if (m_state_handler != nullptr) {
            delete m_state_handler;
        }
        
        if (m_window_assigner_context != nullptr) {
            delete m_window_assigner_context;
        }

        if (m_trigger_context != nullptr) {
            delete m_trigger_context;
        }

        delete m_time_service_manager;
    }

    void open() override {
        m_timestamped_collector = std::make_shared<TimestampedCollector<OUT>>(this->m_output);

        _internal_timer_service = &get_internal_timer_service("window-timers", *this);

        m_process_context = new WindowOperatorWindowContext<K, IN, W>(
                nullptr, 
                this->m_state_handler->get_keyed_state_backend(),
                this->m_state_handler->get_execution_config(),
                *_internal_timer_service);

        m_trigger_context = new WindowOperatorContext<K, IN, W>(
                this->m_state_handler->get_keyed_state_backend().get_current_key(), // key
                nullptr, // window
                *_internal_timer_service, // internal timer service
                this->m_trigger); // Trigger


        m_window_assigner_context = new WindowOperatorWindowAssignerContext<W>(*_internal_timer_service);

        // m_window_state = (InternalListState<K, W, IN>&)get_or_create_keyed_state(m_window_state_descriptor);

        // TODO: implement merging window assigner
    }

    void process_element(StreamRecordV2<IN>* element) override {
        std::vector<W> element_windows = m_window_assigner.assign_windows(&element->val, element->timestamp, *m_window_assigner_context);

        ConstParamK key = this->get_keyed_state_backend().get_current_key();

        // if element is handled by none of assigned element_windows
        bool is_skipped_element = true;

        // TODO: implement merging window
        for (int i = 0; i < element_windows.size(); i++) {
            std::cout << "process_element(), assigning window: " << element_windows[i].to_string() << std::endl;
            // drop if the window is already late
            if (is_window_late(element_windows[i])) {
                continue;
            }
            is_skipped_element = false;
            m_window_state.set_current_namespace(element_windows[i]);
            m_window_state.add(element->val);

            m_trigger_context->set_key(key);
            m_trigger_context->set_window(element_windows[i]);

            TriggerResult trigger_result = m_trigger_context->on_element(element);

            if (trigger_result == TriggerResult::FIRE || trigger_result == TriggerResult::FIRE_AND_PURGE) {
                if (m_window_state.contains_list()) {
                    ConstParamACC contents = m_window_state.get();
                    // TODO: emit window_result:
                    std::cout << "process_element() emit_window_contents(): " << StringUtils::vec_to_string<ACC>(contents) << std::endl;
                    // emit_window_contents(element_windows[i], contents);
                }
            }

            if (trigger_result == PURGE || trigger_result == FIRE_AND_PURGE) {
                m_window_state.clear();
            }
            // register_clearup_timer(element_windows[i]);
        }
        std::cout << "finish process_element" << std::endl;
    }

    // function in Triggerable
    void on_event_time(const InternalTimer<K, W>& timer) override {
        m_trigger_context->set_key(timer.get_key());
        m_trigger_context->set_window(timer.get_namespace());

        m_window_state.set_current_namespace(m_trigger_context->get_window());

        TriggerResult trigger_result = m_trigger_context->on_event_time(timer.get_timestamp());

        if (trigger_result == TriggerResult::FIRE || trigger_result == TriggerResult::FIRE_AND_PURGE) {
            if (m_window_state.contains_list()) {
                ConstParamACC contents = m_window_state.get();
                std::cout << "on_event_time() emit_window_contents(): " << StringUtils::vec_to_string<ACC>(contents) << std::endl;
                // emit_window_contents(timer.get_namespace(), contents);
            }
        }

        if (trigger_result == PURGE || trigger_result == FIRE_AND_PURGE) {
            m_window_state.clear();
        }
    }

    void on_processing_time(const InternalTimer<K, W>& timer) override {
        std::cout << "WindowOperator::on_processing_time(), timestamp:" << TimeUtil::current_timestamp() << ", window: " << timer.get_namespace().to_string() << std::endl;
        m_trigger_context->set_key(timer.get_key());
        m_trigger_context->set_window(timer.get_namespace());

        m_window_state.set_current_namespace(m_trigger_context->get_window());

        TriggerResult trigger_result = m_trigger_context->on_processing_time(timer.get_timestamp());

        if (trigger_result == TriggerResult::FIRE || trigger_result == TriggerResult::FIRE_AND_PURGE) {
            if (m_window_state.contains_list()) {
                ConstParamACC contents = m_window_state.get();
                std::cout << "on_processing_time() emit_window_contents(): " << StringUtils::vec_to_string<ACC>(contents) << std::endl;
                // emit_window_contents(timer.get_namespace(), contents);
            }
        }

        if (trigger_result == PURGE || trigger_result == FIRE_AND_PURGE) {
            m_window_state.clear();
        }
    }

    bool is_window_late(W& window) {
        return (m_window_assigner.is_event_time() && (window.max_timestamp() <= _internal_timer_service->current_watermark()));
    }

    // -------------------------------------------------------------
    // Functions that ought to be placed in AbstractStreamOperator
    // -------------------------------------------------------------

    InternalTimerService<W>& get_internal_timer_service(const std::string& name, Triggerable<K, W>& triggerable) {
        return m_time_service_manager->get_internal_timer_service(name, triggerable);
    }

    ListState<IN>& get_or_create_keyed_state(const StateDescriptor<ListState<IN>, std::vector<int>>& window_state_descriptor) {
        return m_state_handler->get_or_create_keyed_state(window_state_descriptor);
    } 

    const AbstractKeyedStateBackendType& get_keyed_state_backend() {
        return m_state_handler->get_keyed_state_backend();
    }

    void set_current_key(ConstParamK key) override {
        // TODO: implement set_current_key()
    }

    ConstParamK get_current_key() {
        // TODO: implement get_current_key()
    }

protected:
    void register_clearup_timer(const W& window) {
        long clearup_time = window.max_timestamp();
        if (clearup_time == LONG_MAX) {
            return;
        }

        if (m_window_assigner.is_event_time()) {
            m_trigger_context->register_event_time_timer(clearup_time);
        } else {
            m_trigger_context->register_processing_time_timer(clearup_time);
        }
    }

private:
    void emit_window_contents(const W& window, ConstParamACC contents) {
        m_timestamped_collector->set_timestamp(window.max_timestamp());
        m_process_context->set_window(window);
        std::shared_ptr<InternalWindowFunction<ACC, OUT, K, W>> window_func = std::dynamic_pointer_cast<InternalWindowFunction<ACC, OUT, K, W>>(this->m_user_function);
        window_func->process(m_trigger_context->get_key(), window, *m_process_context, contents, m_timestamped_collector);
    }
};
