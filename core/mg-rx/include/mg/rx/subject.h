/**
 * @file subject.h
 * @brief Hot observables (Subject and BehaviorSubject) for event broadcasting.
 */
#ifndef MG_RX_SUBJECT_H
#define MG_RX_SUBJECT_H

#include "mg/rx/observable.h"
#include "mg/pal/thread.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
typedef struct mg_subject mg_subject_t;
typedef struct mg_behavior_subject mg_behavior_subject_t;

/* --- Standard Hot Subject --- */

/**
 * @brief Creates a new hot subject.
 * @return Pointer to subject, or NULL on failure.
 */
mg_subject_t* mg_subject_create(void);

/**
 * @brief Destroys a subject and disposes all active subscriptions.
 * @param s Pointer to subject. Safe to pass NULL.
 */
void mg_subject_destroy(mg_subject_t* s);

/**
 * @brief Subscribes a listener to the subject.
 * @param s Pointer to subject.
 * @param on_next Callback to invoke on emit.
 * @param ctx User context passed to on_next.
 * @return Subscription handle.
 */
mg_subscription_t* mg_subject_subscribe(mg_subject_t* s, mg_rx_next_fn on_next, void* ctx);

/**
 * @brief Emits a value to all active subscribers.
 * @param s Pointer to subject.
 * @param value Pointer/data to emit.
 */
void mg_subject_emit(mg_subject_t* s, void* value);

/**
 * @brief Returns an observable view of this subject.
 * @param s Pointer to subject.
 * @return Observable handle (owned by the subject).
 */
mg_observable_t* mg_subject_as_observable(mg_subject_t* s);

/**
 * @brief Returns the number of currently active subscribers.
 */
size_t mg_subject_subscriber_count(const mg_subject_t* s);

/* --- Behavior Subject --- */

/**
 * @brief Creates a behavior subject holding an initial value.
 * @param initial_value Pointer to initial value.
 * @return Pointer to behavior subject, or NULL on failure.
 */
mg_behavior_subject_t* mg_behavior_subject_create(void* initial_value);

/**
 * @brief Destroys a behavior subject and disposes all subscriptions.
 * @param bs Pointer to behavior subject. Safe to pass NULL.
 */
void mg_behavior_subject_destroy(mg_behavior_subject_t* bs);

/**
 * @brief Subscribes to the behavior subject. Immediately emits the current value.
 * @param bs Pointer to behavior subject.
 * @param on_next Callback to invoke on emit.
 * @param ctx User context passed to on_next.
 * @return Subscription handle.
 */
mg_subscription_t* mg_behavior_subject_subscribe(mg_behavior_subject_t* bs, mg_rx_next_fn on_next, void* ctx);

/**
 * @brief Emits a new value, updating the stored value and notifying subscribers.
 * @param bs Pointer to behavior subject.
 * @param value New value.
 */
void mg_behavior_subject_emit(mg_behavior_subject_t* bs, void* value);

/**
 * @brief Returns the current value held by the behavior subject.
 * @param bs Pointer to behavior subject.
 * @return Current value pointer.
 */
void* mg_behavior_subject_get_value(const mg_behavior_subject_t* bs);

/**
 * @brief Returns an observable view of this behavior subject.
 * @param bs Pointer to behavior subject.
 * @return Observable handle (owned by the behavior subject).
 */
mg_observable_t* mg_behavior_subject_as_observable(mg_behavior_subject_t* bs);

#ifdef __cplusplus
}
#endif

#endif /* MG_RX_SUBJECT_H */
