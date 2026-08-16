/**
 * @file test_mg_alloc.c
 * @brief Headless unit tests for mg-alloc (arena, pool, and ring_buffer).
 */
#include "mg/alloc/alloc.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static int test_arena(void) {
    printf("Testing mg_arena...\n");
    mg_arena_t* arena = mg_arena_create(1024);
    if (!arena) {
        fprintf(stderr, "FAIL: mg_arena_create returned NULL\n");
        return 1;
    }

    /* Test simple allocation */
    int* val = (int*)mg_arena_alloc(arena, sizeof(int), sizeof(int));
    if (!val) {
        fprintf(stderr, "FAIL: mg_arena_alloc failed\n");
        mg_arena_destroy(arena);
        return 1;
    }
    *val = 42;

    /* Test zeroed allocation */
    char* zeros = (char*)mg_arena_alloc_zero(arena, 64, 16);
    if (!zeros) {
        fprintf(stderr, "FAIL: mg_arena_alloc_zero failed\n");
        mg_arena_destroy(arena);
        return 1;
    }
    for (int i = 0; i < 64; ++i) {
        if (zeros[i] != 0) {
            fprintf(stderr, "FAIL: memory not zeroed\n");
            mg_arena_destroy(arena);
            return 1;
        }
    }

    if (mg_arena_get_used(arena) == 0) {
        fprintf(stderr, "FAIL: used count is 0\n");
        mg_arena_destroy(arena);
        return 1;
    }

    size_t high_water = mg_arena_get_high_water_mark(arena);
    if (high_water == 0) {
        fprintf(stderr, "FAIL: high water mark is 0\n");
        mg_arena_destroy(arena);
        return 1;
    }

    /* Test reset */
    mg_arena_reset(arena);
    if (mg_arena_get_used(arena) != 0) {
        fprintf(stderr, "FAIL: arena offset not reset to 0\n");
        mg_arena_destroy(arena);
        return 1;
    }

    /* High water mark should persist across reset */
    if (mg_arena_get_high_water_mark(arena) != high_water) {
        fprintf(stderr, "FAIL: high water mark corrupted across reset\n");
        mg_arena_destroy(arena);
        return 1;
    }

    mg_arena_destroy(arena);
    printf("PASS: mg_arena\n");
    return 0;
}

static int test_pool(void) {
    printf("Testing mg_pool...\n");
    typedef struct {
        float x, y, z;
        uint32_t id;
    } test_component_t;

    const size_t block_count = 16;
    mg_pool_t* pool = mg_pool_create(sizeof(test_component_t), block_count, 8);
    if (!pool) {
        fprintf(stderr, "FAIL: mg_pool_create returned NULL\n");
        return 1;
    }

    if (mg_pool_get_capacity(pool) != block_count) {
        fprintf(stderr, "FAIL: capacity mismatch\n");
        mg_pool_destroy(pool);
        return 1;
    }

    test_component_t* items[16];
    for (size_t i = 0; i < block_count; ++i) {
        items[i] = (test_component_t*)mg_pool_alloc(pool);
        if (!items[i]) {
            fprintf(stderr, "FAIL: pool allocation %zu failed\n", i);
            mg_pool_destroy(pool);
            return 1;
        }
        items[i]->id = (uint32_t)i;
    }

    if (mg_pool_get_allocated_count(pool) != block_count) {
        fprintf(stderr, "FAIL: allocated count mismatch\n");
        mg_pool_destroy(pool);
        return 1;
    }

    /* Next allocation must fail (exhausted) */
    void* overflow = mg_pool_alloc(pool);
    if (overflow != NULL) {
        fprintf(stderr, "FAIL: pool should be exhausted\n");
        mg_pool_destroy(pool);
        return 1;
    }

    /* Free some blocks */
    mg_pool_free(pool, items[3]);
    mg_pool_free(pool, items[7]);
    if (mg_pool_get_allocated_count(pool) != block_count - 2) {
        fprintf(stderr, "FAIL: allocated count not updated after free\n");
        mg_pool_destroy(pool);
        return 1;
    }

    /* Allocate again */
    test_component_t* reallocated = (test_component_t*)mg_pool_alloc(pool);
    if (!reallocated) {
        fprintf(stderr, "FAIL: reallocation failed\n");
        mg_pool_destroy(pool);
        return 1;
    }

    mg_pool_destroy(pool);
    printf("PASS: mg_pool\n");
    return 0;
}

static int test_ring_buffer(void) {
    printf("Testing mg_ring_buffer...\n");
    const size_t capacity = 64;
    mg_ring_buffer_t* rb = mg_ring_buffer_create(capacity);
    if (!rb) {
        fprintf(stderr, "FAIL: mg_ring_buffer_create returned NULL\n");
        return 1;
    }

    const char msg1[] = "Hello MultiGUIEngine!";
    size_t len1 = strlen(msg1);

    size_t pushed = mg_ring_buffer_push(rb, msg1, len1);
    if (pushed != len1) {
        fprintf(stderr, "FAIL: pushed %zu expected %zu\n", pushed, len1);
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    if (mg_ring_buffer_available_read(rb) != len1) {
        fprintf(stderr, "FAIL: available read mismatch\n");
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    char out_buf[128] = {0};
    size_t popped = mg_ring_buffer_pop(rb, out_buf, len1);
    if (popped != len1 || strcmp(out_buf, msg1) != 0) {
        fprintf(stderr, "FAIL: pop mismatch\n");
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    if (mg_ring_buffer_available_read(rb) != 0) {
        fprintf(stderr, "FAIL: buffer should be empty\n");
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    /* Test wrapping around ring buffer boundary */
    char chunk[40];
    memset(chunk, 'A', sizeof(chunk));
    mg_ring_buffer_push(rb, chunk, 40);
    mg_ring_buffer_pop(rb, out_buf, 40);

    memset(chunk, 'B', sizeof(chunk));
    pushed = mg_ring_buffer_push(rb, chunk, 40); /* Wraps around */
    if (pushed != 40) {
        fprintf(stderr, "FAIL: wrapped push failed\n");
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    popped = mg_ring_buffer_pop(rb, out_buf, 40);
    if (popped != 40 || memcmp(out_buf, chunk, 40) != 0) {
        fprintf(stderr, "FAIL: wrapped pop mismatch\n");
        mg_ring_buffer_destroy(rb);
        return 1;
    }

    mg_ring_buffer_destroy(rb);
    printf("PASS: mg_ring_buffer\n");
    return 0;
}

int main(void) {
    if (test_arena() != 0) return 1;
    if (test_pool() != 0) return 1;
    if (test_ring_buffer() != 0) return 1;
    printf("All mg-alloc unit tests passed!\n");
    return 0;
}
