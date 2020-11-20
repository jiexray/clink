#pragma once

/**
  Result Type for trigger methods. This determines what happens with the window.
 */
enum TriggerResult {
    /**
      No actionis taken on the window.
     */
    CONTINUE,

    /**
      Evaluates the window function and emits the window result.
     */
    FIRE_AND_PURGE,

    /**
      The window is evaluated and results are emitted.
     */
    FIRE,

    /**
      All elements in the window are cleared and the window is discarded,
      without evaluating the window function or emitting any elements.
     */
    PURGE
};