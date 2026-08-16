/**
 * @file bus.h
 * @brief Thread-safe event bus bridging background producer threads with main-thread reactive streams.
 */
#ifndef MG_RX_BUS_H
#define MG_RX_BUS_H

#include "mg/rx/subject.h"
#include "mg/alloc/ring_buffer.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_rx_bus mg_rx_bus_t;

/**
 * @brief Header prefixed to each raw event packet in the ring buffer.
 */
typedef struct mg_rx_bus_packet {
    uint32_t payload_size;
} mg_rx_bus_packet_t;

/**
 * @brief Creates a thread-safe event bus with an internal SPSC ring buffer.
 * @param ring_buffer_capacity Capacity in bytes (must be power of two).
 * @return Pointer to bus, or NULL on failure.
 */
mg_rx_bus_t* mg_rx_bus_create(size_t ring_buffer_capacity);

/**
 * @brief Destroys an event bus.
 * @param bus Pointer to bus. Safe to pass NULL.
 */
void mg_rx_bus_destroy(mg_rx_bus_t* bus);

/**
 * @brief Pushes an event onto the bus from a producer thread (lock-free).
 * @param bus Pointer to bus.
 * @param data Event payload pointer.
 * @param size Byte size of event payload.
 * @return True on success, false if ring buffer is full.
 */
bool mg_rx_bus_push(mg_rx_bus_t* bus, const void* data, size_t size);

/**
 * @brief Drains all queued events on the main thread and re-emits them on the subject.
 * @note Must be called once per frame (e.g. at the start of INPUT stage).
 * @param bus Pointer to bus.
 * @return Number of events drained and emitted.
 */
size_t mg_rx_bus_drain(mg_rx_bus_t* bus);

/**
 * @brief Returns an observable representation of the event bus.
 * @param bus Pointer to bus.
 * @return Observable stream handle.
 */
mg_observable_t* mg_rx_bus_as_observable(mg_rx_bus_t* bus);

/**
 * @brief Subscribes to events drained by the bus.
 * @param bus Pointer to bus.
 * @param on_next Callback to invoke for each drained event.
 * @param ctx Context pointer.
 * @return Subscription handle.
 */
mg_subscription_t* mg_rx_bus_subscribe(mg_rx_bus_t* bus, mg_rx_next_fn on_next, void* ctx);

#ifdef __cplusplus
}
#endif

#endif /* MG_RX_BUS_H */
