/**
 * @file time.h
 * @brief High-resolution monotonic timing and sleeping utilities.
 */
#ifndef MG_PAL_TIME_H
#define MG_PAL_TIME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns current monotonic time in nanoseconds.
 * @note Guaranteed to be strictly non-decreasing across calls.
 */
uint64_t mg_time_now_ns(void);

/**
 * @brief Returns current monotonic time in seconds (as double precision float).
 */
double mg_time_now_sec(void);

/**
 * @brief Returns elapsed time in seconds since start_ns.
 * @param start_ns Timestamp previously obtained from mg_time_now_ns().
 */
double mg_time_since_sec(uint64_t start_ns);

/**
 * @brief Suspends current thread execution for at least ms milliseconds.
 * @param ms Duration in milliseconds.
 */
void mg_time_sleep_ms(uint32_t ms);

/**
 * @brief Suspends current thread execution for at least ns nanoseconds.
 * @param ns Duration in nanoseconds.
 */
void mg_time_sleep_ns(uint64_t ns);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_TIME_H */
