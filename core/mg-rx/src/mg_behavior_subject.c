/**
 * @file mg_behavior_subject.c
 * @brief BehaviorSubject implementation.
 */
#include "mg/rx/subject.h"
#include <stdlib.h>

struct mg_behavior_subject {
    mg_subject_t* subject;
    void* current_value;
    mg_observable_t observable_view;
};

static mg_subscription_t* mg_behavior_subject_observable_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_behavior_subject_t* bs = (mg_behavior_subject_t*)obs->node_data;
    return mg_behavior_subject_subscribe(bs, on_next, ctx);
}

mg_behavior_subject_t* mg_behavior_subject_create(void* initial_value) {
    mg_behavior_subject_t* bs = (mg_behavior_subject_t*)malloc(sizeof(mg_behavior_subject_t));
    if (!bs) return NULL;

    bs->subject = mg_subject_create();
    if (!bs->subject) {
        free(bs);
        return NULL;
    }

    bs->current_value = initial_value;

    bs->observable_view.subscribe_fn = mg_behavior_subject_observable_subscribe;
    bs->observable_view.destroy_fn = NULL;
    bs->observable_view.node_data = bs;
    bs->observable_view.is_view_only = true;

    return bs;
}

void mg_behavior_subject_destroy(mg_behavior_subject_t* bs) {
    if (!bs) return;
    if (bs->subject) {
        mg_subject_destroy(bs->subject);
    }
    free(bs);
}

mg_subscription_t* mg_behavior_subject_subscribe(
    mg_behavior_subject_t* bs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    if (!bs || !on_next) return NULL;

    /* Subscribe to underlying subject */
    mg_subscription_t* sub = mg_subject_subscribe(bs->subject, on_next, ctx);
    if (!sub) return NULL;

    /* Immediately replay the current value to the new subscriber */
    on_next(bs->current_value, ctx);

    return sub;
}

void mg_behavior_subject_emit(mg_behavior_subject_t* bs, void* value) {
    if (!bs) return;
    bs->current_value = value;
    mg_subject_emit(bs->subject, value);
}

void* mg_behavior_subject_get_value(const mg_behavior_subject_t* bs) {
    return bs ? bs->current_value : NULL;
}

mg_observable_t* mg_behavior_subject_as_observable(mg_behavior_subject_t* bs) {
    return bs ? &bs->observable_view : NULL;
}
