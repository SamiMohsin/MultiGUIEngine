/**
 * @file pool.h
 * @brief Fixed-size block pool allocator with O(1) allocation and deallocation.
 */
#ifndef MG_ALLOC_POOL_H
#define MG_ALLOC_POOL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Default block alignment (8 bytes). */
#define MG_POOL_DEFAULT_ALIGNMENT 8

/**
 * @brief Internal node structure for the free list.
 */
typedef struct mg_pool_node {
    struct mg_pool_node* next;
} mg_pool_node_t;

/**
 * @brief Fixed-size block pool allocator.
 */
typedef struct mg_pool {
    uint8_t* buffer;           /**< Pointer to contiguous memory buffer. */
    size_t block_size;         /**< Usable size of each block in bytes (aligned). */
    size_t block_count;        /**< Total number of blocks. */
    size_t allocated_count;    /**< Number of currently active allocations. */
    size_t alignment;          /**< Alignment in bytes for each block. */
    mg_pool_node_t* free_list; /**< Head pointer to free list. */
    bool owns_buffer;          /**< True if pool owns its backing memory. */
} mg_pool_t;

/**
 * @brief Initializes a fixed block pool from caller-supplied memory.
 * @param pool Pointer to pool struct to initialize. Must not be NULL.
 * @param backing_memory Pre-allocated buffer. Must not be NULL.
 * @param block_size Size of each individual element in bytes.
 * @param block_count Total count of elements.
 * @param alignment Alignment in bytes (power of two, or 0 for default).
 * @return True if initialized successfully, false otherwise.
 */
bool mg_pool_init(mg_pool_t* pool, void* backing_memory, size_t block_size, size_t block_count, size_t alignment);

/**
 * @brief Dynamically creates a new pool with heap-allocated backing memory.
 * @param block_size Size of each individual element in bytes.
 * @param block_count Total count of elements.
 * @param alignment Alignment in bytes (power of two, or 0 for default).
 * @return Pointer to initialized pool, or NULL on failure.
 */
mg_pool_t* mg_pool_create(size_t block_size, size_t block_count, size_t alignment);

/**
 * @brief Destroys a pool, releasing owned resources.
 * @param pool Pointer to pool. Safe to pass NULL.
 */
void mg_pool_destroy(mg_pool_t* pool);

/**
 * @brief Allocates one block from the pool in O(1) time.
 * @param pool Pointer to pool. Must not be NULL.
 * @return Pointer to aligned block, or NULL if pool is exhausted.
 */
void* mg_pool_alloc(mg_pool_t* pool);

/**
 * @brief Frees a previously allocated block back to the pool in O(1) time.
 * @param pool Pointer to pool. Must not be NULL.
 * @param ptr Pointer to block previously returned by mg_pool_alloc.
 */
void mg_pool_free(mg_pool_t* pool, void* ptr);

/**
 * @brief Resets the pool free list, invalidating all current allocations.
 * @param pool Pointer to pool. Must not be NULL.
 */
void mg_pool_reset(mg_pool_t* pool);

/**
 * @brief Returns the number of currently allocated blocks.
 * @param pool Pointer to pool.
 * @return Number of active allocations.
 */
size_t mg_pool_get_allocated_count(const mg_pool_t* pool);

/**
 * @brief Returns the total capacity of the pool in blocks.
 * @param pool Pointer to pool.
 * @return Total block capacity.
 */
size_t mg_pool_get_capacity(const mg_pool_t* pool);

#ifdef __cplusplus
}
#endif

#endif /* MG_ALLOC_POOL_H */
