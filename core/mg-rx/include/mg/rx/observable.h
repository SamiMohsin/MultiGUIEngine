/**
 * @file observable.h
 * @brief Observable pipeline interfaces.
 */
#ifndef MG_RX_OBSERVABLE_H
#define MG_RX_OBSERVABLE_H

#include "mg/rx/subscription.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_observable mg_observable_t;

/**
 * @brief Function pointer representing an observable subscription action.
 */
typedef mg_subscription_t* (*mg_rx_subscribe_fn)(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
);

/**
 * @brief Custom destructor function for an observable node.
 */
typedef void (*mg_rx_observable_destroy_fn)(mg_observable_t* obs);

/**
 * @brief Read-only stream pipeline node.
 */
struct mg_observable {
    mg_rx_subscribe_fn subscribe_fn;
    mg_rx_observable_destroy_fn destroy_fn;
    void* node_data;
    bool is_view_only; /* If true, destroy_fn is not owned by the observable wrapper */
};

/**
 * @brief Subscribes a consumer callback to an observable stream.
 * @param obs Pointer to observable. Must not be NULL.
 * @param on_next Callback to invoke for each emitted value.
 * @param ctx Caller-supplied context pointer passed to on_next.
 * @return Pointer to subscription handle. Must be disposed via mg_subscription_dispose().
 */
mg_subscription_t* mg_observable_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
);

/**
 * @brief Destroys an observable pipeline node.
 * @note Does not dispose active subscriptions. Call only when tearing down the pipeline.
 * @param obs Pointer to observable. Safe to pass NULL.
 */
void mg_observable_destroy(mg_observable_t* obs);

#ifdef __cplusplus
}
#endif

#endif /* MG_RX_OBSERVABLE_H */
