/**
 * @file thread.h
 * @brief Cross-platform threading, mutex, and condition variable primitives.
 */
#ifndef MG_PAL_THREAD_H
#define MG_PAL_THREAD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque thread handle */
typedef struct mg_thread mg_thread_t;

/* Opaque mutex handle */
typedef struct mg_mutex mg_mutex_t;

/* Opaque condition variable handle */
typedef struct mg_condvar mg_condvar_t;

/** Thread entry function pointer. */
typedef void (*mg_thread_fn)(void* user_data);

/* --- Threads --- */

/**
 * @brief Spawns a new platform thread.
 * @param fn Thread function to execute.
 * @param user_data Argument passed to fn.
 * @param name Optional thread debug name (can be NULL).
 * @return Pointer to thread handle, or NULL on failure.
 */
mg_thread_t* mg_thread_spawn(mg_thread_fn fn, void* user_data, const char* name);

/**
 * @brief Waits for a spawned thread to finish and destroys the thread handle.
 * @param thread Pointer to thread handle. Safe to pass NULL.
 */
void mg_thread_join(mg_thread_t* thread);

/**
 * @brief Yields processor execution to another thread.
 */
void mg_thread_yield(void);

/**
 * @brief Returns current thread unique identifier.
 */
uint64_t mg_thread_get_current_id(void);

/**
 * @brief Returns number of hardware CPU cores / hyperthreads available.
 */
uint32_t mg_thread_get_hardware_concurrency(void);

/* --- Mutexes --- */

/**
 * @brief Creates and initializes a mutual exclusion lock.
 * @return Pointer to initialized mutex, or NULL on failure.
 */
mg_mutex_t* mg_mutex_create(void);

/**
 * @brief Destroys a mutex.
 * @param mutex Pointer to mutex. Safe to pass NULL.
 */
void mg_mutex_destroy(mg_mutex_t* mutex);

/**
 * @brief Acquires lock on mutex, blocking until acquired.
 * @param mutex Pointer to mutex.
 */
void mg_mutex_lock(mg_mutex_t* mutex);

/**
 * @brief Attempts to acquire lock without blocking.
 * @param mutex Pointer to mutex.
 * @return True if lock was acquired, false if busy.
 */
bool mg_mutex_try_lock(mg_mutex_t* mutex);

/**
 * @brief Releases lock on mutex.
 * @param mutex Pointer to mutex.
 */
void mg_mutex_unlock(mg_mutex_t* mutex);

/* --- Condition Variables --- */

/**
 * @brief Creates and initializes a condition variable.
 * @return Pointer to condition variable, or NULL on failure.
 */
mg_condvar_t* mg_condvar_create(void);

/**
 * @brief Destroys a condition variable.
 * @param cv Pointer to condition variable. Safe to pass NULL.
 */
void mg_condvar_destroy(mg_condvar_t* cv);

/**
 * @brief Atomically unlocks mutex and waits for condition variable to be signaled.
 * @param cv Pointer to condition variable.
 * @param mutex Pointer to locked mutex.
 */
void mg_condvar_wait(mg_condvar_t* cv, mg_mutex_t* mutex);

/**
 * @brief Waits on condition variable with timeout in milliseconds.
 * @param cv Pointer to condition variable.
 * @param mutex Pointer to locked mutex.
 * @param timeout_ms Timeout duration in milliseconds.
 * @return True if signaled, false if timed out.
 */
bool mg_condvar_wait_timeout_ms(mg_condvar_t* cv, mg_mutex_t* mutex, uint32_t timeout_ms);

/**
 * @brief Unblocks at least one thread waiting on condition variable.
 * @param cv Pointer to condition variable.
 */
void mg_condvar_signal(mg_condvar_t* cv);

/**
 * @brief Unblocks all threads waiting on condition variable.
 * @param cv Pointer to condition variable.
 */
void mg_condvar_broadcast(mg_condvar_t* cv);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_THREAD_H */
