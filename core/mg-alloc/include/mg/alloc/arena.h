/**
 * @file arena.h
 * @brief Linear bump allocator for high-performance per-frame allocations.
 */
#ifndef MG_ALLOC_ARENA_H
#define MG_ALLOC_ARENA_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Default alignment for arena allocations (16 bytes for SIMD compatibility). */
#define MG_ARENA_DEFAULT_ALIGNMENT 16

/**
 * @brief Linear bump arena allocator.
 */
typedef struct mg_arena {
    uint8_t* buffer;           /**< Pointer to contiguous memory buffer. */
    size_t capacity;           /**< Total capacity in bytes. */
    size_t offset;             /**< Current allocation offset in bytes. */
    size_t high_water_mark;    /**< Maximum bytes allocated since last full clear. */
    bool owns_buffer;          /**< True if the arena allocated its own backing memory. */
} mg_arena_t;

/**
 * @brief Initializes an arena using an existing caller-provided memory buffer.
 * @param arena Pointer to the arena struct to initialize. Must not be NULL.
 * @param backing_memory Pointer to pre-allocated contiguous memory. Must not be NULL.
 * @param capacity Size of the backing buffer in bytes.
 * @return True if initialized successfully, false otherwise.
 */
bool mg_arena_init(mg_arena_t* arena, void* backing_memory, size_t capacity);

/**
 * @brief Dynamically creates a new arena with heap-allocated backing memory.
 * @param capacity Total capacity in bytes.
 * @return Pointer to initialized arena, or NULL on allocation failure.
 */
mg_arena_t* mg_arena_create(size_t capacity);

/**
 * @brief Destroys an arena, freeing owned memory if applicable.
 * @param arena Pointer to arena to destroy. Safe to pass NULL.
 */
void mg_arena_destroy(mg_arena_t* arena);

/**
 * @brief Allocates a block of memory from the arena.
 * @param arena Pointer to arena. Must not be NULL.
 * @param size Number of bytes to allocate.
 * @param alignment Required alignment (must be a power of two, or 0 for default).
 * @return Pointer to aligned allocated block, or NULL if out of memory.
 */
void* mg_arena_alloc(mg_arena_t* arena, size_t size, size_t alignment);

/**
 * @brief Allocates a zero-initialized block of memory from the arena.
 * @param arena Pointer to arena. Must not be NULL.
 * @param size Number of bytes to allocate.
 * @param alignment Required alignment (must be a power of two, or 0 for default).
 * @return Pointer to aligned zeroed memory block, or NULL if out of memory.
 */
void* mg_arena_alloc_zero(mg_arena_t* arena, size_t size, size_t alignment);

/**
 * @brief Resets the arena's allocation offset back to zero.
 * @note This is typically called once per frame at the end of the frame loop.
 * @param arena Pointer to arena. Must not be NULL.
 */
void mg_arena_reset(mg_arena_t* arena);

/**
 * @brief Gets the number of currently used bytes in the arena.
 * @param arena Pointer to arena.
 * @return Number of allocated bytes.
 */
size_t mg_arena_get_used(const mg_arena_t* arena);

/**
 * @brief Gets the total capacity of the arena in bytes.
 * @param arena Pointer to arena.
 * @return Total capacity in bytes.
 */
size_t mg_arena_get_capacity(const mg_arena_t* arena);

/**
 * @brief Gets the high-water mark of allocated bytes.
 * @param arena Pointer to arena.
 * @return Peak byte usage.
 */
size_t mg_arena_get_high_water_mark(const mg_arena_t* arena);

#ifdef __cplusplus
}
#endif

#endif /* MG_ALLOC_ARENA_H */
