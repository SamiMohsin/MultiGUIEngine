/**
 * @file mg_bus.c
 * @brief Thread-safe event bus implementation.
 */
#include "mg/rx/bus.h"
#include <stdlib.h>
#include <string.h>

#define MG_RX_BUS_MAX_EVENT_SIZE 4096

struct mg_rx_bus {
    mg_ring_buffer_t* ring_buffer;
    mg_subject_t* subject;
    uint8_t drain_scratch[MG_RX_BUS_MAX_EVENT_SIZE];
};

mg_rx_bus_t* mg_rx_bus_create(size_t ring_buffer_capacity) {
    mg_rx_bus_t* bus = (mg_rx_bus_t*)malloc(sizeof(mg_rx_bus_t));
    if (!bus) return NULL;

    bus->ring_buffer = mg_ring_buffer_create(ring_buffer_capacity);
    if (!bus->ring_buffer) {
        free(bus);
        return NULL;
    }

    bus->subject = mg_subject_create();
    if (!bus->subject) {
        mg_ring_buffer_destroy(bus->ring_buffer);
        free(bus);
        return NULL;
    }

    return bus;
}

void mg_rx_bus_destroy(mg_rx_bus_t* bus) {
    if (!bus) return;
    if (bus->subject) {
        mg_subject_destroy(bus->subject);
    }
    if (bus->ring_buffer) {
        mg_ring_buffer_destroy(bus->ring_buffer);
    }
    free(bus);
}

bool mg_rx_bus_push(mg_rx_bus_t* bus, const void* data, size_t size) {
    if (!bus || !data || size == 0 || size > MG_RX_BUS_MAX_EVENT_SIZE) {
        return false;
    }

    size_t total_needed = sizeof(mg_rx_bus_packet_t) + size;
    if (mg_ring_buffer_available_write(bus->ring_buffer) < total_needed) {
        return false;
    }

    mg_rx_bus_packet_t header;
    header.payload_size = (uint32_t)size;

    size_t header_written = mg_ring_buffer_push(bus->ring_buffer, &header, sizeof(header));
    if (header_written != sizeof(header)) {
        return false;
    }

    size_t payload_written = mg_ring_buffer_push(bus->ring_buffer, data, size);
    if (payload_written != size) {
        return false;
    }

    return true;
}

size_t mg_rx_bus_drain(mg_rx_bus_t* bus) {
    if (!bus) return 0;
    size_t drained_count = 0;

    while (mg_ring_buffer_available_read(bus->ring_buffer) >= sizeof(mg_rx_bus_packet_t)) {
        mg_rx_bus_packet_t header;
        size_t popped_header = mg_ring_buffer_pop(bus->ring_buffer, &header, sizeof(header));
        if (popped_header != sizeof(header)) {
            break;
        }

        if (header.payload_size > MG_RX_BUS_MAX_EVENT_SIZE) {
            break; /* Corrupt packet */
        }

        size_t popped_payload = mg_ring_buffer_pop(bus->ring_buffer, bus->drain_scratch, header.payload_size);
        if (popped_payload != header.payload_size) {
            break;
        }

        mg_subject_emit(bus->subject, bus->drain_scratch);
        drained_count++;
    }

    return drained_count;
}

mg_observable_t* mg_rx_bus_as_observable(mg_rx_bus_t* bus) {
    return bus ? mg_subject_as_observable(bus->subject) : NULL;
}

mg_subscription_t* mg_rx_bus_subscribe(mg_rx_bus_t* bus, mg_rx_next_fn on_next, void* ctx) {
    return bus ? mg_subject_subscribe(bus->subject, on_next, ctx) : NULL;
}
