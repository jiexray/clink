#pragma once

/**
  A Window is a grouping of elements into finite buckets. Windows have a maximum timestamp 
  which means that, at some point, all elements that go into one window will have arrived.
 */

class Window {
public:
    virtual long max_timestamp() const = 0;
};

