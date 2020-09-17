/**
 * A buffer manager takes charge of the buffer releasing.
 */
#pragma once
#include "BufferPool.hpp"
#include "BufferBase.hpp"
#include "Buffer.hpp"
#include "ReadOnlySlidedBuffer.hpp"
#include "BufferBuilder.hpp"
#include "BufferConsumer.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <map>
#include <mutex>

class Buffer;
class BufferPool;

/**
 * Note: Register/Unregister of BufferConsumer, BufferBase (Buffer slices) and Buffer can be accessed from
 *       upstream StreamTask and downstream StreamTask. All three parts need locks.
 */

class BufferPoolManager
{
private:
    /* Map from buffer_id to number of working BufferConsumers */
    std::map<int, int>              m_buffer_id_to_num_consumers;
    /* Map from buffer_id to number of un-read slices */
    std::map<int, int>              m_buffer_id_to_num_slices;
    /* Map from buffer_id to Buffer */
    std::map<int, Buffer*>          m_buffer_id_to_buffer;
    /* The BufferPool which is managed by this BufferPoolManager */
    BufferPool*                     m_buffer_pool;

    std::mutex                      m_buffer_manager_mtx; // a global mutex

    static std::shared_ptr<spdlog::logger> m_logger;

public:
    BufferPoolManager(BufferPool* buffer_pool):m_buffer_pool(buffer_pool) {}

    // -------------------------------------------
    // Register and unregister for BufferConsumer
    // -------------------------------------------
    void                    register_buffer_consumer(int buffer_id);
    void                    unregister_buffer_consumer(int buffer_id);

    // -------------------------------------------------
    // Register and unregister for ReadOnlySlidedBuffer
    // -------------------------------------------------
    void                    register_buffer_slice(int buffer_id);
    void                    unregister_buffer_slice(int buffer_id);

    void                    register_buffer(Buffer* buffer);

    /* Put Buffer back to BufferPool */
    void                    release_buffer(int buffer_id);

    /* Properties */
    int                     get_num_consumers(int buffer_id) {return m_buffer_id_to_num_consumers.find(buffer_id) == m_buffer_id_to_num_consumers.end()?
                                                                        -1: m_buffer_id_to_num_consumers[buffer_id];}

    int                     get_num_slices(int buffer_id) {return m_buffer_id_to_num_slices.find(buffer_id) == m_buffer_id_to_num_slices.end()?
                                                                        -1: m_buffer_id_to_num_slices[buffer_id];}
};

