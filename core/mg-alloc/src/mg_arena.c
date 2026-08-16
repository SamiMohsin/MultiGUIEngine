/**
 * @file mg_arena.c
 * @brief Linear bump allocator implementation.
 */
#include "mg/alloc/arena.h"
#include <stdlib.h>
#include <string.h>

static inline uintptr_t mg_align_forward(uintptr_t ptr, size_t alignment) {
    if (alignment == 0) {
        return ptr;
    }
    size_t remainder = ptr % alignment;
    if (remainder == 0) {
        return ptr;
    }
    return ptr + (alignment - remainder);
}

bool mg_arena_init(mg_arena_t* arena, void* backing_memory, size_t capacity) {
    if (arena == NULL || backing_memory == NULL || capacity == 0) {
        return false;
    }
    arena->buffer = (uint8_t*)backing_memory;
    arena->capacity = capacity;
    arena->offset = 0;
    arena->high_water_mark = 0;
    arena->owns_buffer = false;
    return true;
}

mg_arena_t* mg_arena_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }
    mg_arena_t* arena = (mg_arena_t*)malloc(sizeof(mg_arena_t));
    if (arena == NULL) {
        return NULL;
    }
    void* buffer = malloc(capacity);
    if (buffer == NULL) {
        free(arena);
        return NULL;
    }
    arena->buffer = (uint8_t*)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
    arena->high_water_mark = 0;
    arena->owns_buffer = true;
    return arena;
}

void mg_arena_destroy(mg_arena_t* arena) {
    if (arena == NULL) {
        return;
    }
    if (arena->owns_buffer && arena->buffer != NULL) {
        free(arena->buffer);
        arena->buffer = NULL;
    }
    free(arena);
}

void* mg_arena_alloc(mg_arena_t* arena, size_t size, size_t alignment) {
    if (arena == NULL || size == 0 || arena->buffer == NULL) {
        return NULL;
    }
    if (alignment == 0) {
        alignment = MG_ARENA_DEFAULT_ALIGNMENT;
    }

    uintptr_t current_ptr = (uintptr_t)(arena->buffer + arena->offset);
    uintptr_t aligned_ptr = mg_align_forward(current_ptr, alignment);
    size_t new_offset = (size_t)(aligned_ptr - (uintptr_t)arena->buffer) + size;

    if (new_offset > arena->capacity) {
        return NULL; /* Out of memory */
    }

    arena->offset = new_offset;
    if (arena->offset > arena->high_water_mark) {
        arena->high_water_mark = arena->offset;
    }

    return (void*)aligned_ptr;
}

void* mg_arena_alloc_zero(mg_arena_t* arena, size_t size, size_t alignment) {
    void* ptr = mg_arena_alloc(arena, size, alignment);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void mg_arena_reset(mg_arena_t* arena) {
    if (arena != NULL) {
        arena->offset = 0;
    }
}

size_t mg_arena_get_used(const mg_arena_t* arena) {
    return arena != NULL ? arena->offset : 0;
}

size_t mg_arena_get_capacity(const mg_arena_t* arena) {
    return arena != NULL ? arena->capacity : 0;
}

size_t mg_arena_get_high_water_mark(const mg_arena_t* arena) {
    return arena != NULL ? arena->high_water_mark : 0;
}
