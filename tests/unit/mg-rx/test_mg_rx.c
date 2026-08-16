/**
 * @file test_mg_rx.c
 * @brief Comprehensive headless unit tests for mg-rx.
 */
#include "mg/rx/rx.h"
#include "mg/pal/time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Test helper types --- */
typedef struct {
    int values[64];
    size_t count;
} int_collector_t;

static void collect_int(void* value, void* ctx) {
    int_collector_t* col = (int_collector_t*)ctx;
    if (col->count < 64) {
        col->values[col->count++] = (int)(intptr_t)value;
    }
}

/* --- 1. Subject Test --- */
static int test_subject(void) {
    printf("Testing mg_subject...\n");
    mg_subject_t* s = mg_subject_create();
    if (!s) return 1;

    int_collector_t col1 = { {0}, 0 };
    int_collector_t col2 = { {0}, 0 };

    mg_subscription_t* sub1 = mg_subject_subscribe(s, collect_int, &col1);
    mg_subscription_t* sub2 = mg_subject_subscribe(s, collect_int, &col2);

    if (mg_subject_subscriber_count(s) != 2) {
        fprintf(stderr, "FAIL: expected 2 subscribers\n");
        return 1;
    }

    mg_subject_emit(s, (void*)(intptr_t)10);
    mg_subject_emit(s, (void*)(intptr_t)20);

    /* Dispose sub1 */
    mg_subscription_dispose(sub1);
    if (mg_subject_subscriber_count(s) != 1) {
        fprintf(stderr, "FAIL: expected 1 subscriber after dispose\n");
        return 1;
    }

    mg_subject_emit(s, (void*)(intptr_t)30);

    mg_subscription_dispose(sub2);
    mg_subject_destroy(s);

    if (col1.count != 2 || col1.values[0] != 10 || col1.values[1] != 20) {
        fprintf(stderr, "FAIL: col1 mismatch\n");
        return 1;
    }

    if (col2.count != 3 || col2.values[0] != 10 || col2.values[1] != 20 || col2.values[2] != 30) {
        fprintf(stderr, "FAIL: col2 mismatch\n");
        return 1;
    }

    printf("PASS: mg_subject\n");
    return 0;
}

/* --- 2. BehaviorSubject Test --- */
static int test_behavior_subject(void) {
    printf("Testing mg_behavior_subject...\n");
    mg_behavior_subject_t* bs = mg_behavior_subject_create((void*)(intptr_t)99);
    if (!bs) return 1;

    int_collector_t col1 = { {0}, 0 };
    /* Upon subscribe, should immediately receive 99 */
    mg_subscription_t* sub1 = mg_behavior_subject_subscribe(bs, collect_int, &col1);

    if (col1.count != 1 || col1.values[0] != 99) {
        fprintf(stderr, "FAIL: initial replay failed\n");
        return 1;
    }

    mg_behavior_subject_emit(bs, (void*)(intptr_t)100);

    int_collector_t col2 = { {0}, 0 };
    /* Sub2 should receive the latest value (100) on subscribe */
    mg_subscription_t* sub2 = mg_behavior_subject_subscribe(bs, collect_int, &col2);

    if (col2.count != 1 || col2.values[0] != 100) {
        fprintf(stderr, "FAIL: second replay failed\n");
        return 1;
    }

    mg_subscription_dispose(sub1);
    mg_subscription_dispose(sub2);
    mg_behavior_subject_destroy(bs);

    printf("PASS: mg_behavior_subject\n");
    return 0;
}

/* --- 3. Map Operator Test --- */
static void* map_multiply_by_two(void* val, void* ctx) {
    (void)ctx;
    int v = (int)(intptr_t)val;
    return (void*)(intptr_t)(v * 2);
}

static int test_map_operator(void) {
    printf("Testing mg_rx_map...\n");
    mg_subject_t* s = mg_subject_create();
    mg_observable_t* mapped = mg_rx_map(mg_subject_as_observable(s), map_multiply_by_two, NULL);

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_observable_subscribe(mapped, collect_int, &col);

    mg_subject_emit(s, (void*)(intptr_t)1);
    mg_subject_emit(s, (void*)(intptr_t)2);
    mg_subject_emit(s, (void*)(intptr_t)3);

    mg_subscription_dispose(sub);
    mg_observable_destroy(mapped);
    mg_subject_destroy(s);

    if (col.count != 3 || col.values[0] != 2 || col.values[1] != 4 || col.values[2] != 6) {
        fprintf(stderr, "FAIL: map output mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_map\n");
    return 0;
}

/* --- 4. Filter Operator Test --- */
static bool filter_even_only(void* val, void* ctx) {
    (void)ctx;
    int v = (int)(intptr_t)val;
    return (v % 2) == 0;
}

static int test_filter_operator(void) {
    printf("Testing mg_rx_filter...\n");
    mg_subject_t* s = mg_subject_create();
    mg_observable_t* filtered = mg_rx_filter(mg_subject_as_observable(s), filter_even_only, NULL);

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_observable_subscribe(filtered, collect_int, &col);

    for (int i = 1; i <= 6; ++i) {
        mg_subject_emit(s, (void*)(intptr_t)i);
    }

    mg_subscription_dispose(sub);
    mg_observable_destroy(filtered);
    mg_subject_destroy(s);

    if (col.count != 3 || col.values[0] != 2 || col.values[1] != 4 || col.values[2] != 6) {
        fprintf(stderr, "FAIL: filter output mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_filter\n");
    return 0;
}

/* --- 5. Merge Operator Test --- */
static int test_merge_operator(void) {
    printf("Testing mg_rx_merge...\n");
    mg_subject_t* s1 = mg_subject_create();
    mg_subject_t* s2 = mg_subject_create();
    mg_observable_t* merged = mg_rx_merge(mg_subject_as_observable(s1), mg_subject_as_observable(s2));

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_observable_subscribe(merged, collect_int, &col);

    mg_subject_emit(s1, (void*)(intptr_t)10);
    mg_subject_emit(s2, (void*)(intptr_t)20);
    mg_subject_emit(s1, (void*)(intptr_t)30);
    mg_subject_emit(s2, (void*)(intptr_t)40);

    mg_subscription_dispose(sub);
    mg_observable_destroy(merged);
    mg_subject_destroy(s1);
    mg_subject_destroy(s2);

    if (col.count != 4 || col.values[0] != 10 || col.values[1] != 20 || col.values[2] != 30 || col.values[3] != 40) {
        fprintf(stderr, "FAIL: merge output mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_merge\n");
    return 0;
}

/* --- 6. Combine Latest Operator Test --- */
static void* combine_add(void* a, void* b, void* ctx) {
    (void)ctx;
    int va = (int)(intptr_t)a;
    int vb = (int)(intptr_t)b;
    return (void*)(intptr_t)(va + vb);
}

static int test_combine_latest_operator(void) {
    printf("Testing mg_rx_combine_latest...\n");
    mg_subject_t* s1 = mg_subject_create();
    mg_subject_t* s2 = mg_subject_create();
    mg_observable_t* combined = mg_rx_combine_latest(
        mg_subject_as_observable(s1),
        mg_subject_as_observable(s2),
        combine_add,
        NULL
    );

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_observable_subscribe(combined, collect_int, &col);

    /* Emit s1=1 -> should not emit yet (s2 hasn't emitted) */
    mg_subject_emit(s1, (void*)(intptr_t)1);
    if (col.count != 0) {
        fprintf(stderr, "FAIL: emitted before both sources fired\n");
        return 1;
    }

    /* Emit s2=10 -> should emit 1 + 10 = 11 */
    mg_subject_emit(s2, (void*)(intptr_t)10);
    /* Emit s1=2 -> should emit 2 + 10 = 12 */
    mg_subject_emit(s1, (void*)(intptr_t)2);
    /* Emit s2=20 -> should emit 2 + 20 = 22 */
    mg_subject_emit(s2, (void*)(intptr_t)20);

    mg_subscription_dispose(sub);
    mg_observable_destroy(combined);
    mg_subject_destroy(s1);
    mg_subject_destroy(s2);

    if (col.count != 3 || col.values[0] != 11 || col.values[1] != 12 || col.values[2] != 22) {
        fprintf(stderr, "FAIL: combine_latest values mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_combine_latest\n");
    return 0;
}

/* --- 7. Scan Operator Test --- */
static void* scan_sum(void* acc, void* val, void* ctx) {
    (void)ctx;
    int a = (int)(intptr_t)acc;
    int v = (int)(intptr_t)val;
    return (void*)(intptr_t)(a + v);
}

static int test_scan_operator(void) {
    printf("Testing mg_rx_scan...\n");
    mg_subject_t* s = mg_subject_create();
    mg_observable_t* scanned = mg_rx_scan(
        mg_subject_as_observable(s),
        (void*)(intptr_t)0,
        scan_sum,
        NULL
    );

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_observable_subscribe(scanned, collect_int, &col);

    mg_subject_emit(s, (void*)(intptr_t)1); /* acc = 0 + 1 = 1 */
    mg_subject_emit(s, (void*)(intptr_t)2); /* acc = 1 + 2 = 3 */
    mg_subject_emit(s, (void*)(intptr_t)3); /* acc = 3 + 3 = 6 */
    mg_subject_emit(s, (void*)(intptr_t)4); /* acc = 6 + 4 = 10 */

    mg_subscription_dispose(sub);
    mg_observable_destroy(scanned);
    mg_subject_destroy(s);

    if (col.count != 4 || col.values[0] != 1 || col.values[1] != 3 || col.values[2] != 6 || col.values[3] != 10) {
        fprintf(stderr, "FAIL: scan output mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_scan\n");
    return 0;
}

/* --- 8. Event Bus Test (Threaded Producer -> Main Thread Drain) --- */
typedef struct {
    mg_rx_bus_t* bus;
    int event_count;
} bus_worker_ctx_t;

typedef struct {
    int id;
    int value;
} custom_event_t;

static void bus_producer_thread(void* arg) {
    bus_worker_ctx_t* ctx = (bus_worker_ctx_t*)arg;
    for (int i = 0; i < ctx->event_count; ++i) {
        custom_event_t ev = { i, i * 10 };
        mg_rx_bus_push(ctx->bus, &ev, sizeof(ev));
    }
}

static void collect_bus_event(void* value, void* ctx) {
    custom_event_t* ev = (custom_event_t*)value;
    int_collector_t* col = (int_collector_t*)ctx;
    if (col->count < 64) {
        col->values[col->count++] = ev->value;
    }
}

static int test_rx_bus(void) {
    printf("Testing mg_rx_bus cross-thread drain...\n");
    mg_rx_bus_t* bus = mg_rx_bus_create(1024);
    if (!bus) return 1;

    int_collector_t col = { {0}, 0 };
    mg_subscription_t* sub = mg_rx_bus_subscribe(bus, collect_bus_event, &col);

    bus_worker_ctx_t worker_ctx = { bus, 5 };
    mg_thread_t* thread = mg_thread_spawn(bus_producer_thread, &worker_ctx, "bus_producer");
    if (!thread) return 1;

    mg_thread_join(thread);

    /* Drain the bus on the main thread */
    size_t drained = mg_rx_bus_drain(bus);
    if (drained != 5) {
        fprintf(stderr, "FAIL: expected 5 drained events, got %zu\n", drained);
        return 1;
    }

    mg_subscription_dispose(sub);
    mg_rx_bus_destroy(bus);

    if (col.count != 5 || col.values[0] != 0 || col.values[1] != 10 || col.values[2] != 20 ||
        col.values[3] != 30 || col.values[4] != 40) {
        fprintf(stderr, "FAIL: drained event payload mismatch\n");
        return 1;
    }

    printf("PASS: mg_rx_bus\n");
    return 0;
}

int main(void) {
    if (test_subject() != 0) return 1;
    if (test_behavior_subject() != 0) return 1;
    if (test_map_operator() != 0) return 1;
    if (test_filter_operator() != 0) return 1;
    if (test_merge_operator() != 0) return 1;
    if (test_combine_latest_operator() != 0) return 1;
    if (test_scan_operator() != 0) return 1;
    if (test_rx_bus() != 0) return 1;

    printf("All mg-rx unit tests passed successfully!\n");
    return 0;
}
