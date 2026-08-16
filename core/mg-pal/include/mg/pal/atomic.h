/**
 * @file atomic.h
 * @brief Cross-platform atomic primitives and memory barriers.
 */
#ifndef MG_PAL_ATOMIC_H
#define MG_PAL_ATOMIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile int32_t  mg_atomic_int32_t;
typedef volatile uint32_t mg_atomic_uint32_t;
typedef volatile int64_t  mg_atomic_int64_t;
typedef volatile uint64_t mg_atomic_uint64_t;
typedef void* volatile    mg_atomic_ptr_t;

/* Memory ordering constants matching C11 / GCC semantics */
#define MG_MEMORY_ORDER_RELAXED __ATOMIC_RELAXED
#define MG_MEMORY_ORDER_ACQUIRE __ATOMIC_ACQUIRE
#define MG_MEMORY_ORDER_RELEASE __ATOMIC_RELEASE
#define MG_MEMORY_ORDER_ACQ_REL __ATOMIC_ACQ_REL
#define MG_MEMORY_ORDER_SEQ_CST __ATOMIC_SEQ_CST

/** Issues a full sequential consistency hardware memory barrier. */
static inline void mg_atomic_fence(void) {
#if defined(_MSC_VER)
    #include <windows.h>
    MemoryBarrier();
#elif defined(__GNUC__) || defined(__clang__)
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
#else
    /* Fallback */
#endif
}

/** Atomic load 32-bit integer. */
static inline int32_t mg_atomic_load_i32(const mg_atomic_int32_t* obj) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_load_n(obj, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return InterlockedCompareExchange((volatile LONG*)obj, 0, 0);
#else
    return *obj;
#endif
}

/** Atomic store 32-bit integer. */
static inline void mg_atomic_store_i32(mg_atomic_int32_t* obj, int32_t val) {
#if defined(__GNUC__) || defined(__clang__)
    __atomic_store_n(obj, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    InterlockedExchange((volatile LONG*)obj, val);
#else
    *obj = val;
#endif
}

/** Atomic fetch and add 32-bit integer. Returns previous value. */
static inline int32_t mg_atomic_fetch_add_i32(mg_atomic_int32_t* obj, int32_t arg) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_fetch_add(obj, arg, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return InterlockedExchangeAdd((volatile LONG*)obj, arg);
#else
    int32_t old = *obj; *obj += arg; return old;
#endif
}

/** Atomic fetch and subtract 32-bit integer. Returns previous value. */
static inline int32_t mg_atomic_fetch_sub_i32(mg_atomic_int32_t* obj, int32_t arg) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_fetch_sub(obj, arg, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return InterlockedExchangeAdd((volatile LONG*)obj, -arg);
#else
    int32_t old = *obj; *obj -= arg; return old;
#endif
}

/**
 * @brief Atomic compare-and-swap (CAS) 32-bit integer.
 * If *obj == *expected, sets *obj = desired and returns true.
 * Otherwise, loads *obj into *expected and returns false.
 */
static inline bool mg_atomic_cas_i32(mg_atomic_int32_t* obj, int32_t* expected, int32_t desired) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    LONG exp = *expected;
    LONG old = InterlockedCompareExchange((volatile LONG*)obj, desired, exp);
    if (old == exp) {
        return true;
    }
    *expected = old;
    return false;
#else
    if (*obj == *expected) {
        *obj = desired;
        return true;
    }
    *expected = *obj;
    return false;
#endif
}

/** Atomic load pointer. */
static inline void* mg_atomic_load_ptr(const mg_atomic_ptr_t* obj) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_load_n(obj, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return InterlockedCompareExchangePointer((void* volatile*)obj, NULL, NULL);
#else
    return *obj;
#endif
}

/** Atomic store pointer. */
static inline void mg_atomic_store_ptr(mg_atomic_ptr_t* obj, void* val) {
#if defined(__GNUC__) || defined(__clang__)
    __atomic_store_n(obj, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    InterlockedExchangePointer((void* volatile*)obj, val);
#else
    *obj = val;
#endif
}

/** Atomic CAS pointer. */
static inline bool mg_atomic_cas_ptr(mg_atomic_ptr_t* obj, void** expected, void* desired) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    void* exp = *expected;
    void* old = InterlockedCompareExchangePointer((void* volatile*)obj, desired, exp);
    if (old == exp) {
        return true;
    }
    *expected = old;
    return false;
#else
    if (*obj == *expected) {
        *obj = desired;
        return true;
    }
    *expected = *obj;
    return false;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_ATOMIC_H */
