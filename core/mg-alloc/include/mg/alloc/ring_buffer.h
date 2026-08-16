/**
 * @file ring_buffer.h
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) ring buffer.
 */
#ifndef MG_ALLOC_RING_BUFFER_H
#define MG_ALLOC_RING_BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Lock-free SPSC circular byte buffer.
 */
typedef struct mg_ring_buffer {
    uint8_t* buffer;           /**< Backing storage array. */
    size_t capacity;           /**< Power-of-two buffer capacity in bytes. */
    size_t mask;               /**< Bitmask for index wrapping (capacity - 1). */
    volatile size_t head;      /**< Producer write index (monotonically increasing). */
    volatile size_t tail;      /**< Consumer read index (monotonically increasing). */
    bool owns_buffer;          /**< True if ring buffer owns backing memory. */
} mg_ring_buffer_t;

/**
 * @brief Initializes a ring buffer from caller-provided memory.
 * @param rb Pointer to ring buffer struct to initialize. Must not be NULL.
 * @param backing_memory Pointer to pre-allocated buffer. Must not be NULL.
 * @param capacity Buffer capacity in bytes (must be a power of two).
 * @return True if initialized successfully, false otherwise.
 */
bool mg_ring_buffer_init(mg_ring_buffer_t* rb, void* backing_memory, size_t capacity);

/**
 * @brief Dynamically creates a new ring buffer with heap backing memory.
 * @param capacity Buffer capacity in bytes (must be a power of two).
 * @return Pointer to initialized ring buffer, or NULL on failure.
 */
mg_ring_buffer_t* mg_ring_buffer_create(size_t capacity);

/**
 * @brief Destroys a ring buffer, releasing owned memory.
 * @param rb Pointer to ring buffer. Safe to pass NULL.
 */
void mg_ring_buffer_destroy(mg_ring_buffer_t* rb);

/**
 * @brief Writes bytes into the ring buffer (called only by producer thread).
 * @param rb Pointer to ring buffer. Must not be NULL.
 * @param data Pointer to input data. Must not be NULL.
 * @param size Number of bytes to push.
 * @return Number of bytes actually written (may be less than size if full).
 */
size_t mg_ring_buffer_push(mg_ring_buffer_t* rb, const void* data, size_t size);

/**
 * @brief Reads bytes from the ring buffer (called only by consumer thread).
 * @param rb Pointer to ring buffer. Must not be NULL.
 * @param out_data Pointer to output buffer. Must not be NULL.
 * @param size Number of bytes to read.
 * @return Number of bytes actually read (may be less than size if empty).
 */
size_t mg_ring_buffer_pop(mg_ring_buffer_t* rb, void* out_data, size_t size);

/**
 * @brief Returns the number of bytes available for reading.
 * @param rb Pointer to ring buffer.
 * @return Available bytes to read.
 */
size_t mg_ring_buffer_available_read(const mg_ring_buffer_t* rb);

/**
 * @brief Returns the free space available for writing in bytes.
 * @param rb Pointer to ring buffer.
 * @return Available bytes to write.
 */
size_t mg_ring_buffer_available_write(const mg_ring_buffer_t* rb);

/**
 * @brief Resets the ring buffer read and write pointers to empty.
 * @note Not thread-safe with active push/pop. Call only when threads are synchronized.
 * @param rb Pointer to ring buffer. Must not be NULL.
 */
void mg_ring_buffer_clear(mg_ring_buffer_t* rb);

#ifdef __cplusplus
}
#endif

#endif /* MG_ALLOC_RING_BUFFER_H */
