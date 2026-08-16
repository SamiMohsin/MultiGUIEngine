/**
 * @file mg_pool.c
 * @brief Fixed-size block pool allocator implementation.
 */
#include "mg/alloc/pool.h"
#include <stdlib.h>
#include <string.h>

static inline size_t mg_align_size(size_t size, size_t alignment) {
    if (alignment == 0) {
        alignment = MG_POOL_DEFAULT_ALIGNMENT;
    }
    size_t remainder = size % alignment;
    if (remainder == 0) {
        return size;
    }
    return size + (alignment - remainder);
}

bool mg_pool_init(mg_pool_t* pool, void* backing_memory, size_t block_size, size_t block_count, size_t alignment) {
    if (pool == NULL || backing_memory == NULL || block_size == 0 || block_count == 0) {
        return false;
    }

    if (alignment == 0) {
        alignment = MG_POOL_DEFAULT_ALIGNMENT;
    }

    /* Ensure block size is at least large enough to hold a free list node pointer */
    if (block_size < sizeof(mg_pool_node_t)) {
        block_size = sizeof(mg_pool_node_t);
    }
    size_t actual_block_size = mg_align_size(block_size, alignment);

    pool->buffer = (uint8_t*)backing_memory;
    pool->block_size = actual_block_size;
    pool->block_count = block_count;
    pool->allocated_count = 0;
    pool->alignment = alignment;
    pool->owns_buffer = false;
    pool->free_list = NULL;

    mg_pool_reset(pool);
    return true;
}

mg_pool_t* mg_pool_create(size_t block_size, size_t block_count, size_t alignment) {
    if (block_size == 0 || block_count == 0) {
        return NULL;
    }

    if (alignment == 0) {
        alignment = MG_POOL_DEFAULT_ALIGNMENT;
    }

    if (block_size < sizeof(mg_pool_node_t)) {
        block_size = sizeof(mg_pool_node_t);
    }
    size_t actual_block_size = mg_align_size(block_size, alignment);
    size_t total_bytes = actual_block_size * block_count;

    mg_pool_t* pool = (mg_pool_t*)malloc(sizeof(mg_pool_t));
    if (pool == NULL) {
        return NULL;
    }

    void* buffer = malloc(total_bytes);
    if (buffer == NULL) {
        free(pool);
        return NULL;
    }

    pool->buffer = (uint8_t*)buffer;
    pool->block_size = actual_block_size;
    pool->block_count = block_count;
    pool->allocated_count = 0;
    pool->alignment = alignment;
    pool->owns_buffer = true;
    pool->free_list = NULL;

    mg_pool_reset(pool);
    return pool;
}

void mg_pool_destroy(mg_pool_t* pool) {
    if (pool == NULL) {
        return;
    }
    if (pool->owns_buffer && pool->buffer != NULL) {
        free(pool->buffer);
        pool->buffer = NULL;
    }
    free(pool);
}

void* mg_pool_alloc(mg_pool_t* pool) {
    if (pool == NULL || pool->free_list == NULL) {
        return NULL; /* Out of blocks */
    }

    mg_pool_node_t* node = pool->free_list;
    pool->free_list = node->next;
    pool->allocated_count++;

    return (void*)node;
}

void mg_pool_free(mg_pool_t* pool, void* ptr) {
    if (pool == NULL || ptr == NULL) {
        return;
    }

    /* Boundary check */
    uint8_t* byte_ptr = (uint8_t*)ptr;
    size_t total_size = pool->block_size * pool->block_count;
    if (byte_ptr < pool->buffer || byte_ptr >= (pool->buffer + total_size)) {
        return; /* Invalid pointer not in pool range */
    }

    mg_pool_node_t* node = (mg_pool_node_t*)ptr;
    node->next = pool->free_list;
    pool->free_list = node;
    if (pool->allocated_count > 0) {
        pool->allocated_count--;
    }
}

void mg_pool_reset(mg_pool_t* pool) {
    if (pool == NULL || pool->buffer == NULL) {
        return;
    }

    pool->free_list = (mg_pool_node_t*)pool->buffer;
    mg_pool_node_t* current = pool->free_list;

    for (size_t i = 0; i < pool->block_count - 1; ++i) {
        mg_pool_node_t* next = (mg_pool_node_t*)(pool->buffer + (i + 1) * pool->block_size);
        current->next = next;
        current = next;
    }
    current->next = NULL;
    pool->allocated_count = 0;
}

size_t mg_pool_get_allocated_count(const mg_pool_t* pool) {
    return pool != NULL ? pool->allocated_count : 0;
}

size_t mg_pool_get_capacity(const mg_pool_t* pool) {
    return pool != NULL ? pool->block_count : 0;
}
