/**
 * @file operators.h
 * @brief Stream transformation, filtering, and combination operators.
 */
#ifndef MG_RX_OPERATORS_H
#define MG_RX_OPERATORS_H

#include "mg/rx/observable.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Transform function for map operator. */
typedef void* (*mg_rx_map_fn)(void* value, void* ctx);

/** Predicate function for filter operator. */
typedef bool (*mg_rx_predicate_fn)(void* value, void* ctx);

/** Combine function for combine_latest operator. */
typedef void* (*mg_rx_combine_fn)(void* val_a, void* val_b, void* ctx);

/** Accumulator function for scan operator. */
typedef void* (*mg_rx_scan_fn)(void* accumulator, void* value, void* ctx);

/**
 * @brief Maps each emitted item to a new value via map_fn.
 * @param source Input stream.
 * @param map_fn Transformation function.
 * @param ctx Context pointer passed to map_fn.
 * @return New observable emitting transformed values.
 */
mg_observable_t* mg_rx_map(mg_observable_t* source, mg_rx_map_fn map_fn, void* ctx);

/**
 * @brief Filters emissions by applying a boolean predicate.
 * @param source Input stream.
 * @param pred_fn Predicate function returning true to pass, false to drop.
 * @param ctx Context pointer passed to pred_fn.
 * @return New observable emitting filtered values.
 */
mg_observable_t* mg_rx_filter(mg_observable_t* source, mg_rx_predicate_fn pred_fn, void* ctx);

/**
 * @brief Emits a notification only after delay_ms has elapsed since the last emission.
 * @param source Input stream.
 * @param delay_ms Milliseconds of silence required before emitting.
 * @return New debounced observable.
 */
mg_observable_t* mg_rx_debounce_ms(mg_observable_t* source, uint32_t delay_ms);

/**
 * @brief Emits the first item, then ignores subsequent items for duration_ms.
 * @param source Input stream.
 * @param duration_ms Milliseconds window to throttle.
 * @return New throttled observable.
 */
mg_observable_t* mg_rx_throttle_ms(mg_observable_t* source, uint32_t duration_ms);

/**
 * @brief Combines multiple observables into one by merging their emissions.
 * @param source_a First stream.
 * @param source_b Second stream.
 * @return New merged observable.
 */
mg_observable_t* mg_rx_merge(mg_observable_t* source_a, mg_observable_t* source_b);

/**
 * @brief When any observable emits an item, combine the latest items emitted by each source.
 * @param source_a First stream.
 * @param source_b Second stream.
 * @param combine_fn Function combining the two latest values into a single output.
 * @param ctx Context pointer passed to combine_fn.
 * @return New combined observable.
 */
mg_observable_t* mg_rx_combine_latest(
    mg_observable_t* source_a,
    mg_observable_t* source_b,
    mg_rx_combine_fn combine_fn,
    void* ctx
);

/**
 * @brief Applies an accumulator function over the source stream and emits each intermediate result.
 * @param source Input stream.
 * @param initial_acc Starting accumulator value.
 * @param scan_fn Accumulator function.
 * @param ctx Context pointer passed to scan_fn.
 * @return New scanned observable.
 */
mg_observable_t* mg_rx_scan(
    mg_observable_t* source,
    void* initial_acc,
    mg_rx_scan_fn scan_fn,
    void* ctx
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RX_OPERATORS_H */
