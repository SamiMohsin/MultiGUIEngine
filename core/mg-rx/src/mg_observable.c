/**
 * @file mg_observable.c
 * @brief Observable pipeline node implementation.
 */
#include "mg/rx/observable.h"
#include <stdlib.h>

mg_subscription_t* mg_observable_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    if (!obs || !obs->subscribe_fn) {
        return NULL;
    }
    return obs->subscribe_fn(obs, on_next, ctx);
}

void mg_observable_destroy(mg_observable_t* obs) {
    if (!obs) return;

    if (obs->destroy_fn != NULL) {
        obs->destroy_fn(obs);
    }

    if (!obs->is_view_only) {
        free(obs);
    }
}
