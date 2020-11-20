#pragma once
#include "Window.hpp"
#include <vector>
#include <string>

class WindowAssignerContext {
public:
    virtual long get_current_processing_time() = 0;
};

/**
  A WindowAssigner assigns zero or more Windows to an element

  @param <T> The type of elements that this WindowAssigner can assign windows to.
  @param <W> The type of Window that this assigner assigns.
 */
template <class T, class W>
class WindowAssigner
{
public:
    /**
      Return a Collection of windows that should be assigned to the elements.

      @param element The element to which windows should be assigned.
      @param timestamp The timestamp of the element.
      @param context The WindowAssignerContext in which the assigner operates.
     */
    virtual std::vector<W> assign_windows(T* element, long timestamp, WindowAssignerContext& context) = 0;

    /**
      Return the default trigger associated with this WindowAssigner.
     */
    virtual WindowAssigner<T, W>* get_default_trigger() = 0;

    /**
      Returns true if elements are assigned to windows based on event time,
      false otherwise.
     */
    virtual bool is_event_time() = 0;

    virtual std::string to_string() = 0;
};

