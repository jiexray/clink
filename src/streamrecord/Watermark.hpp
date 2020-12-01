#pragma once
#include "StreamRecordV2.hpp"

/**
 * A watermark tells operators that no elements with a timestamp older or equal
 * to the watermark timestamp should arrive at the operator. Watermarks are emitted 
 * at the sources and propagate through the opeators of the topology. Operators must
 * themselves emit watermarks to downstream operators using Output::Watermark(). Operators 
 * that do not internally bufrfer elements can always forward the watermark that they receive.
 * Operators that buffer elements, such as window operators, must forward a watermark after emission
 * of elements that is triggered by the arriving watermark.
 */
struct Watermark: public StreamRecordV2<long> {
    Watermark(long watermark): StreamRecordV2<long>(watermark) {
        type = StreamRecordType::WATERMARK;
    }
};