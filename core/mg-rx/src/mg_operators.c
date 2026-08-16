/**
 * @file mg_operators.c
 * @brief Stream transformation, filtering, and combination operators implementation.
 */
#include "mg/rx/operators.h"
#include "mg/pal/time.h"
#include <stdlib.h>

/* ========================================================================= */
/* Map Operator                                                              */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source;
    mg_rx_map_fn map_fn;
    void* ctx;
} mg_op_map_data_t;

typedef struct {
    mg_subscription_t* upstream_sub;
    mg_rx_map_fn map_fn;
    void* map_ctx;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_map_sub_state_t;

static void mg_op_map_on_next(void* value, void* ctx) {
    mg_op_map_sub_state_t* state = (mg_op_map_sub_state_t*)ctx;
    void* mapped = state->map_fn(value, state->map_ctx);
    state->downstream_next(mapped, state->downstream_ctx);
}

static void mg_op_map_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_map_sub_state_t* state = (mg_op_map_sub_state_t*)sub->internal_state;
    if (state->upstream_sub) {
        mg_subscription_dispose(state->upstream_sub);
        state->upstream_sub = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_map_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_map_data_t* data = (mg_op_map_data_t*)obs->node_data;
    mg_op_map_sub_state_t* state = (mg_op_map_sub_state_t*)malloc(sizeof(mg_op_map_sub_state_t));
    if (!state) return NULL;

    state->map_fn = data->map_fn;
    state->map_ctx = data->ctx;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_map_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->upstream_sub = mg_observable_subscribe(data->source, mg_op_map_on_next, state);
    return downstream_sub;
}

static void mg_op_map_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_map(mg_observable_t* source, mg_rx_map_fn map_fn, void* ctx) {
    if (!source || !map_fn) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_map_data_t* data = (mg_op_map_data_t*)malloc(sizeof(mg_op_map_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source = source;
    data->map_fn = map_fn;
    data->ctx = ctx;

    obs->subscribe_fn = mg_op_map_subscribe;
    obs->destroy_fn = mg_op_map_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Filter Operator                                                           */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source;
    mg_rx_predicate_fn pred_fn;
    void* ctx;
} mg_op_filter_data_t;

typedef struct {
    mg_subscription_t* upstream_sub;
    mg_rx_predicate_fn pred_fn;
    void* pred_ctx;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_filter_sub_state_t;

static void mg_op_filter_on_next(void* value, void* ctx) {
    mg_op_filter_sub_state_t* state = (mg_op_filter_sub_state_t*)ctx;
    if (state->pred_fn(value, state->pred_ctx)) {
        state->downstream_next(value, state->downstream_ctx);
    }
}

static void mg_op_filter_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_filter_sub_state_t* state = (mg_op_filter_sub_state_t*)sub->internal_state;
    if (state->upstream_sub) {
        mg_subscription_dispose(state->upstream_sub);
        state->upstream_sub = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_filter_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_filter_data_t* data = (mg_op_filter_data_t*)obs->node_data;
    mg_op_filter_sub_state_t* state = (mg_op_filter_sub_state_t*)malloc(sizeof(mg_op_filter_sub_state_t));
    if (!state) return NULL;

    state->pred_fn = data->pred_fn;
    state->pred_ctx = data->ctx;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_filter_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->upstream_sub = mg_observable_subscribe(data->source, mg_op_filter_on_next, state);
    return downstream_sub;
}

static void mg_op_filter_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_filter(mg_observable_t* source, mg_rx_predicate_fn pred_fn, void* ctx) {
    if (!source || !pred_fn) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_filter_data_t* data = (mg_op_filter_data_t*)malloc(sizeof(mg_op_filter_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source = source;
    data->pred_fn = pred_fn;
    data->ctx = ctx;

    obs->subscribe_fn = mg_op_filter_subscribe;
    obs->destroy_fn = mg_op_filter_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Throttle Operator                                                         */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source;
    uint32_t duration_ms;
} mg_op_throttle_data_t;

typedef struct {
    mg_subscription_t* upstream_sub;
    uint32_t duration_ms;
    uint64_t last_emit_ns;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_throttle_sub_state_t;

static void mg_op_throttle_on_next(void* value, void* ctx) {
    mg_op_throttle_sub_state_t* state = (mg_op_throttle_sub_state_t*)ctx;
    uint64_t now_ns = mg_time_now_ns();
    uint64_t window_ns = (uint64_t)state->duration_ms * 1000000ULL;

    if (state->last_emit_ns == 0 || (now_ns - state->last_emit_ns) >= window_ns) {
        state->last_emit_ns = now_ns;
        state->downstream_next(value, state->downstream_ctx);
    }
}

static void mg_op_throttle_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_throttle_sub_state_t* state = (mg_op_throttle_sub_state_t*)sub->internal_state;
    if (state->upstream_sub) {
        mg_subscription_dispose(state->upstream_sub);
        state->upstream_sub = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_throttle_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_throttle_data_t* data = (mg_op_throttle_data_t*)obs->node_data;
    mg_op_throttle_sub_state_t* state = (mg_op_throttle_sub_state_t*)malloc(sizeof(mg_op_throttle_sub_state_t));
    if (!state) return NULL;

    state->duration_ms = data->duration_ms;
    state->last_emit_ns = 0;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_throttle_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->upstream_sub = mg_observable_subscribe(data->source, mg_op_throttle_on_next, state);
    return downstream_sub;
}

static void mg_op_throttle_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_throttle_ms(mg_observable_t* source, uint32_t duration_ms) {
    if (!source) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_throttle_data_t* data = (mg_op_throttle_data_t*)malloc(sizeof(mg_op_throttle_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source = source;
    data->duration_ms = duration_ms;

    obs->subscribe_fn = mg_op_throttle_subscribe;
    obs->destroy_fn = mg_op_throttle_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Debounce Operator                                                         */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source;
    uint32_t delay_ms;
} mg_op_debounce_data_t;

typedef struct {
    mg_subscription_t* upstream_sub;
    uint32_t delay_ms;
    uint64_t last_event_ns;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_debounce_sub_state_t;

static void mg_op_debounce_on_next(void* value, void* ctx) {
    mg_op_debounce_sub_state_t* state = (mg_op_debounce_sub_state_t*)ctx;
    uint64_t now_ns = mg_time_now_ns();
    uint64_t window_ns = (uint64_t)state->delay_ms * 1000000ULL;

    if (state->last_event_ns == 0 || (now_ns - state->last_event_ns) >= window_ns) {
        state->last_event_ns = now_ns;
        state->downstream_next(value, state->downstream_ctx);
    } else {
        state->last_event_ns = now_ns;
    }
}

static void mg_op_debounce_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_debounce_sub_state_t* state = (mg_op_debounce_sub_state_t*)sub->internal_state;
    if (state->upstream_sub) {
        mg_subscription_dispose(state->upstream_sub);
        state->upstream_sub = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_debounce_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_debounce_data_t* data = (mg_op_debounce_data_t*)obs->node_data;
    mg_op_debounce_sub_state_t* state = (mg_op_debounce_sub_state_t*)malloc(sizeof(mg_op_debounce_sub_state_t));
    if (!state) return NULL;

    state->delay_ms = data->delay_ms;
    state->last_event_ns = 0;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_debounce_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->upstream_sub = mg_observable_subscribe(data->source, mg_op_debounce_on_next, state);
    return downstream_sub;
}

static void mg_op_debounce_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_debounce_ms(mg_observable_t* source, uint32_t delay_ms) {
    if (!source) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_debounce_data_t* data = (mg_op_debounce_data_t*)malloc(sizeof(mg_op_debounce_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source = source;
    data->delay_ms = delay_ms;

    obs->subscribe_fn = mg_op_debounce_subscribe;
    obs->destroy_fn = mg_op_debounce_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Merge Operator                                                            */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source_a;
    mg_observable_t* source_b;
} mg_op_merge_data_t;

typedef struct {
    mg_subscription_t* sub_a;
    mg_subscription_t* sub_b;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_merge_sub_state_t;

static void mg_op_merge_on_next(void* value, void* ctx) {
    mg_op_merge_sub_state_t* state = (mg_op_merge_sub_state_t*)ctx;
    state->downstream_next(value, state->downstream_ctx);
}

static void mg_op_merge_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_merge_sub_state_t* state = (mg_op_merge_sub_state_t*)sub->internal_state;
    if (state->sub_a) {
        mg_subscription_dispose(state->sub_a);
        state->sub_a = NULL;
    }
    if (state->sub_b) {
        mg_subscription_dispose(state->sub_b);
        state->sub_b = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_merge_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_merge_data_t* data = (mg_op_merge_data_t*)obs->node_data;
    mg_op_merge_sub_state_t* state = (mg_op_merge_sub_state_t*)malloc(sizeof(mg_op_merge_sub_state_t));
    if (!state) return NULL;

    state->sub_a = NULL;
    state->sub_b = NULL;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_merge_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->sub_a = mg_observable_subscribe(data->source_a, mg_op_merge_on_next, state);
    state->sub_b = mg_observable_subscribe(data->source_b, mg_op_merge_on_next, state);
    return downstream_sub;
}

static void mg_op_merge_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_merge(mg_observable_t* source_a, mg_observable_t* source_b) {
    if (!source_a || !source_b) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_merge_data_t* data = (mg_op_merge_data_t*)malloc(sizeof(mg_op_merge_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source_a = source_a;
    data->source_b = source_b;

    obs->subscribe_fn = mg_op_merge_subscribe;
    obs->destroy_fn = mg_op_merge_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Combine Latest Operator                                                   */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source_a;
    mg_observable_t* source_b;
    mg_rx_combine_fn combine_fn;
    void* ctx;
} mg_op_combine_data_t;

typedef struct {
    mg_subscription_t* sub_a;
    mg_subscription_t* sub_b;
    mg_rx_combine_fn combine_fn;
    void* combine_ctx;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;

    bool has_a;
    void* val_a;
    bool has_b;
    void* val_b;
} mg_op_combine_sub_state_t;

static void mg_op_combine_on_next_a(void* value, void* ctx) {
    mg_op_combine_sub_state_t* state = (mg_op_combine_sub_state_t*)ctx;
    state->has_a = true;
    state->val_a = value;
    if (state->has_a && state->has_b) {
        void* combined = state->combine_fn(state->val_a, state->val_b, state->combine_ctx);
        state->downstream_next(combined, state->downstream_ctx);
    }
}

static void mg_op_combine_on_next_b(void* value, void* ctx) {
    mg_op_combine_sub_state_t* state = (mg_op_combine_sub_state_t*)ctx;
    state->has_b = true;
    state->val_b = value;
    if (state->has_a && state->has_b) {
        void* combined = state->combine_fn(state->val_a, state->val_b, state->combine_ctx);
        state->downstream_next(combined, state->downstream_ctx);
    }
}

static void mg_op_combine_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_combine_sub_state_t* state = (mg_op_combine_sub_state_t*)sub->internal_state;
    if (state->sub_a) {
        mg_subscription_dispose(state->sub_a);
        state->sub_a = NULL;
    }
    if (state->sub_b) {
        mg_subscription_dispose(state->sub_b);
        state->sub_b = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_combine_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_combine_data_t* data = (mg_op_combine_data_t*)obs->node_data;
    mg_op_combine_sub_state_t* state = (mg_op_combine_sub_state_t*)malloc(sizeof(mg_op_combine_sub_state_t));
    if (!state) return NULL;

    state->sub_a = NULL;
    state->sub_b = NULL;
    state->combine_fn = data->combine_fn;
    state->combine_ctx = data->ctx;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;
    state->has_a = false;
    state->val_a = NULL;
    state->has_b = false;
    state->val_b = NULL;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_combine_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->sub_a = mg_observable_subscribe(data->source_a, mg_op_combine_on_next_a, state);
    state->sub_b = mg_observable_subscribe(data->source_b, mg_op_combine_on_next_b, state);
    return downstream_sub;
}

static void mg_op_combine_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_combine_latest(
    mg_observable_t* source_a,
    mg_observable_t* source_b,
    mg_rx_combine_fn combine_fn,
    void* ctx
) {
    if (!source_a || !source_b || !combine_fn) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_combine_data_t* data = (mg_op_combine_data_t*)malloc(sizeof(mg_op_combine_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source_a = source_a;
    data->source_b = source_b;
    data->combine_fn = combine_fn;
    data->ctx = ctx;

    obs->subscribe_fn = mg_op_combine_subscribe;
    obs->destroy_fn = mg_op_combine_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}

/* ========================================================================= */
/* Scan Operator                                                             */
/* ========================================================================= */

typedef struct {
    mg_observable_t* source;
    void* initial_acc;
    mg_rx_scan_fn scan_fn;
    void* ctx;
} mg_op_scan_data_t;

typedef struct {
    mg_subscription_t* upstream_sub;
    void* current_acc;
    mg_rx_scan_fn scan_fn;
    void* scan_ctx;
    mg_rx_next_fn downstream_next;
    void* downstream_ctx;
} mg_op_scan_sub_state_t;

static void mg_op_scan_on_next(void* value, void* ctx) {
    mg_op_scan_sub_state_t* state = (mg_op_scan_sub_state_t*)ctx;
    state->current_acc = state->scan_fn(state->current_acc, value, state->scan_ctx);
    state->downstream_next(state->current_acc, state->downstream_ctx);
}

static void mg_op_scan_on_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_op_scan_sub_state_t* state = (mg_op_scan_sub_state_t*)sub->internal_state;
    if (state->upstream_sub) {
        mg_subscription_dispose(state->upstream_sub);
        state->upstream_sub = NULL;
    }
    free(state);
}

static mg_subscription_t* mg_op_scan_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_op_scan_data_t* data = (mg_op_scan_data_t*)obs->node_data;
    mg_op_scan_sub_state_t* state = (mg_op_scan_sub_state_t*)malloc(sizeof(mg_op_scan_sub_state_t));
    if (!state) return NULL;

    state->current_acc = data->initial_acc;
    state->scan_fn = data->scan_fn;
    state->scan_ctx = data->ctx;
    state->downstream_next = on_next;
    state->downstream_ctx = ctx;

    mg_subscription_t* downstream_sub = mg_subscription_create(
        on_next,
        ctx,
        mg_op_scan_on_dispose,
        state
    );
    if (!downstream_sub) {
        free(state);
        return NULL;
    }

    state->upstream_sub = mg_observable_subscribe(data->source, mg_op_scan_on_next, state);
    return downstream_sub;
}

static void mg_op_scan_destroy(mg_observable_t* obs) {
    if (obs && obs->node_data) {
        free(obs->node_data);
    }
}

mg_observable_t* mg_rx_scan(
    mg_observable_t* source,
    void* initial_acc,
    mg_rx_scan_fn scan_fn,
    void* ctx
) {
    if (!source || !scan_fn) return NULL;
    mg_observable_t* obs = (mg_observable_t*)malloc(sizeof(mg_observable_t));
    if (!obs) return NULL;

    mg_op_scan_data_t* data = (mg_op_scan_data_t*)malloc(sizeof(mg_op_scan_data_t));
    if (!data) {
        free(obs);
        return NULL;
    }
    data->source = source;
    data->initial_acc = initial_acc;
    data->scan_fn = scan_fn;
    data->ctx = ctx;

    obs->subscribe_fn = mg_op_scan_subscribe;
    obs->destroy_fn = mg_op_scan_destroy;
    obs->node_data = data;
    obs->is_view_only = false;
    return obs;
}
