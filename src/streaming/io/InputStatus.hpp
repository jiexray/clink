/**
 * An InputStatus indicated the availablility of data from an asynchronous input.
 */
#pragma once

enum InputStatus {
    MORE_AVAILABLE,      // Indicator that more data is available 
                         // and the input can be called immediately again to produce more data
    NOTHING_AVAILABLE,   // Indicator that no data is currently available,
                         // but more data will be available in the future again.
    END_OF_INPUT         // Indicator that the input has reached the end of input
};