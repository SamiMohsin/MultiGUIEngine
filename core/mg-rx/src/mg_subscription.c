/**
 * @file mg_subscription.c
 * @brief Subscription management implementation.
 */
#include "mg/rx/subscription.h"
#include <stdlib.h>
#include <string.h>

mg_subscription_t* mg_subscription_create(
    mg_rx_next_fn on_next,
    void* ctx,
    mg_rx_dispose_fn on_dispose,
    void* internal_state
) {
    mg_subscription_t* sub = (mg_subscription_t*)malloc(sizeof(mg_subscription_t));
    if (!sub) return NULL;

    sub->on_next = on_next;
    sub->ctx = ctx;
    sub->on_dispose = on_dispose;
    sub->internal_state = internal_state;
    sub->is_disposed = false;
    sub->next = NULL;
    sub->prev = NULL;
    return sub;
}

void mg_subscription_dispose(mg_subscription_t* sub) {
    if (!sub || sub->is_disposed) {
        return;
    }

    sub->is_disposed = true;

    if (sub->on_dispose != NULL) {
        sub->on_dispose(sub);
    }

    free(sub);
}

bool mg_subscription_is_active(const mg_subscription_t* sub) {
    return sub != NULL && !sub->is_disposed;
}
