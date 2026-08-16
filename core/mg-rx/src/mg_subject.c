/**
 * @file mg_subject.c
 * @brief Hot Subject implementation.
 */
#include "mg/rx/subject.h"
#include <stdlib.h>
#include <string.h>

struct mg_subject {
    mg_subscription_t* head;
    mg_subscription_t* tail;
    size_t count;
    mg_observable_t observable_view;
};

static void mg_subject_on_subscription_dispose(mg_subscription_t* sub) {
    if (!sub || !sub->internal_state) return;
    mg_subject_t* s = (mg_subject_t*)sub->internal_state;

    if (sub->prev) {
        sub->prev->next = sub->next;
    } else {
        s->head = sub->next;
    }

    if (sub->next) {
        sub->next->prev = sub->prev;
    } else {
        s->tail = sub->prev;
    }

    if (s->count > 0) {
        s->count--;
    }
}

static mg_subscription_t* mg_subject_observable_subscribe(
    mg_observable_t* obs,
    mg_rx_next_fn on_next,
    void* ctx
) {
    mg_subject_t* s = (mg_subject_t*)obs->node_data;
    return mg_subject_subscribe(s, on_next, ctx);
}

mg_subject_t* mg_subject_create(void) {
    mg_subject_t* s = (mg_subject_t*)malloc(sizeof(mg_subject_t));
    if (!s) return NULL;

    s->head = NULL;
    s->tail = NULL;
    s->count = 0;

    s->observable_view.subscribe_fn = mg_subject_observable_subscribe;
    s->observable_view.destroy_fn = NULL;
    s->observable_view.node_data = s;
    s->observable_view.is_view_only = true;

    return s;
}

void mg_subject_destroy(mg_subject_t* s) {
    if (!s) return;

    mg_subscription_t* curr = s->head;
    while (curr) {
        mg_subscription_t* next = curr->next;
        curr->is_disposed = true;
        free(curr);
        curr = next;
    }

    free(s);
}

mg_subscription_t* mg_subject_subscribe(mg_subject_t* s, mg_rx_next_fn on_next, void* ctx) {
    if (!s || !on_next) return NULL;

    mg_subscription_t* sub = mg_subscription_create(
        on_next,
        ctx,
        mg_subject_on_subscription_dispose,
        s
    );
    if (!sub) return NULL;

    if (!s->head) {
        s->head = sub;
        s->tail = sub;
    } else {
        s->tail->next = sub;
        sub->prev = s->tail;
        s->tail = sub;
    }
    s->count++;

    return sub;
}

void mg_subject_emit(mg_subject_t* s, void* value) {
    if (!s) return;

    mg_subscription_t* curr = s->head;
    while (curr) {
        mg_subscription_t* next = curr->next; /* in case callback disposes */
        if (!curr->is_disposed && curr->on_next) {
            curr->on_next(value, curr->ctx);
        }
        curr = next;
    }
}

mg_observable_t* mg_subject_as_observable(mg_subject_t* s) {
    return s ? &s->observable_view : NULL;
}

size_t mg_subject_subscriber_count(const mg_subject_t* s) {
    return s ? s->count : 0;
}
