/**
 * @file subscription.h
 * @brief Subscription lifecycle management for observable streams.
 */
#ifndef MG_RX_SUBSCRIPTION_H
#define MG_RX_SUBSCRIPTION_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
typedef struct mg_subscription mg_subscription_t;

/** Callback invoked when an observable emits a new value. */
typedef void (*mg_rx_next_fn)(void* value, void* ctx);

/** Callback invoked when a subscription is disposed. */
typedef void (*mg_rx_dispose_fn)(mg_subscription_t* sub);

/**
 * @brief Subscription state handle.
 */
struct mg_subscription {
    mg_rx_next_fn on_next;         /**< Consumer value handler. */
    void* ctx;                     /**< Consumer context pointer. */
    mg_rx_dispose_fn on_dispose;   /**< Custom teardown handler. */
    void* internal_state;          /**< Observable / operator internal state. */
    bool is_disposed;              /**< True if subscription has been disposed. */
    struct mg_subscription* next;  /**< Linked list pointer for subscriber lists. */
    struct mg_subscription* prev;  /**< Linked list pointer for subscriber lists. */
};

/**
 * @brief Allocates and initializes a new subscription.
 * @param on_next Value callback.
 * @param ctx User context.
 * @param on_dispose Unsubscribe/teardown callback.
 * @param internal_state Internal operator/source state.
 * @return Pointer to initialized subscription.
 */
mg_subscription_t* mg_subscription_create(
    mg_rx_next_fn on_next,
    void* ctx,
    mg_rx_dispose_fn on_dispose,
    void* internal_state
);

/**
 * @brief Disposes a subscription, detaching it from the stream and freeing resources.
 * @param sub Pointer to subscription handle. Safe to pass NULL.
 */
void mg_subscription_dispose(mg_subscription_t* sub);

/**
 * @brief Checks if a subscription is currently active (not disposed).
 */
bool mg_subscription_is_active(const mg_subscription_t* sub);

#ifdef __cplusplus
}
#endif

#endif /* MG_RX_SUBSCRIPTION_H */
