/**
 * @file mg_ring_buffer.c
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) ring buffer implementation.
 */
#include "mg/alloc/ring_buffer.h"
#include <stdlib.h>
#include <string.h>

/* Memory barriers for cross-compiler support */
#if defined(_MSC_VER)
    #include <windows.h>
    #define MG_MEMORY_BARRIER() MemoryBarrier()
#elif defined(__GNUC__) || defined(__clang__)
    #define MG_MEMORY_BARRIER() __sync_synchronize()
#else
    #define MG_MEMORY_BARRIER() ((void)0)
#endif

static inline bool mg_is_power_of_two(size_t n) {
    return (n != 0) && ((n & (n - 1)) == 0);
}

static inline size_t mg_min_size(size_t a, size_t b) {
    return (a < b) ? a : b;
}

bool mg_ring_buffer_init(mg_ring_buffer_t* rb, void* backing_memory, size_t capacity) {
    if (rb == NULL || backing_memory == NULL || !mg_is_power_of_two(capacity)) {
        return false;
    }

    rb->buffer = (uint8_t*)backing_memory;
    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->head = 0;
    rb->tail = 0;
    rb->owns_buffer = false;
    return true;
}

mg_ring_buffer_t* mg_ring_buffer_create(size_t capacity) {
    if (!mg_is_power_of_two(capacity)) {
        return NULL;
    }

    mg_ring_buffer_t* rb = (mg_ring_buffer_t*)malloc(sizeof(mg_ring_buffer_t));
    if (rb == NULL) {
        return NULL;
    }

    void* buffer = malloc(capacity);
    if (buffer == NULL) {
        free(rb);
        return NULL;
    }

    rb->buffer = (uint8_t*)buffer;
    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->head = 0;
    rb->tail = 0;
    rb->owns_buffer = true;
    return rb;
}

void mg_ring_buffer_destroy(mg_ring_buffer_t* rb) {
    if (rb == NULL) {
        return;
    }
    if (rb->owns_buffer && rb->buffer != NULL) {
        free(rb->buffer);
        rb->buffer = NULL;
    }
    free(rb);
}

size_t mg_ring_buffer_available_read(const mg_ring_buffer_t* rb) {
    if (rb == NULL) {
        return 0;
    }
    size_t head = rb->head;
    size_t tail = rb->tail;
    return head - tail;
}

size_t mg_ring_buffer_available_write(const mg_ring_buffer_t* rb) {
    if (rb == NULL) {
        return 0;
    }
    size_t head = rb->head;
    size_t tail = rb->tail;
    return rb->capacity - (head - tail);
}

size_t mg_ring_buffer_push(mg_ring_buffer_t* rb, const void* data, size_t size) {
    if (rb == NULL || data == NULL || size == 0) {
        return 0;
    }

    size_t head = rb->head;
    size_t tail = rb->tail;
    size_t free_space = rb->capacity - (head - tail);
    size_t to_write = mg_min_size(size, free_space);

    if (to_write == 0) {
        return 0;
    }

    const uint8_t* src = (const uint8_t*)data;
    size_t offset = head & rb->mask;
    size_t first_chunk = mg_min_size(to_write, rb->capacity - offset);
    size_t second_chunk = to_write - first_chunk;

    memcpy(rb->buffer + offset, src, first_chunk);
    if (second_chunk > 0) {
        memcpy(rb->buffer, src + first_chunk, second_chunk);
    }

    MG_MEMORY_BARRIER();
    rb->head = head + to_write;

    return to_write;
}

size_t mg_ring_buffer_pop(mg_ring_buffer_t* rb, void* out_data, size_t size) {
    if (rb == NULL || out_data == NULL || size == 0) {
        return 0;
    }

    size_t head = rb->head;
    size_t tail = rb->tail;
    size_t available = head - tail;
    size_t to_read = mg_min_size(size, available);

    if (to_read == 0) {
        return 0;
    }

    uint8_t* dst = (uint8_t*)out_data;
    size_t offset = tail & rb->mask;
    size_t first_chunk = mg_min_size(to_read, rb->capacity - offset);
    size_t second_chunk = to_read - first_chunk;

    memcpy(dst, rb->buffer + offset, first_chunk);
    if (second_chunk > 0) {
        memcpy(dst + first_chunk, rb->buffer, second_chunk);
    }

    MG_MEMORY_BARRIER();
    rb->tail = tail + to_read;

    return to_read;
}

void mg_ring_buffer_clear(mg_ring_buffer_t* rb) {
    if (rb != NULL) {
        rb->head = 0;
        rb->tail = 0;
    }
}
