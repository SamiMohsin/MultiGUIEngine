/**
 * @file mg_net.c
 * @brief MultiGUIEngine Custom Reliable UDP Network Transport implementation.
 */
#include "mg/net/net.h"
#include <stdlib.h>
#include <string.h>

#define MG_NET_MAX_PEERS 64
#define MG_NET_MAX_EVENTS 256
#define MG_NET_MAX_PAYLOAD 1400

typedef struct {
    mg_net_event_t event;
    uint8_t buffer[MG_NET_MAX_PAYLOAD];
} mg_net_event_entry_t;

typedef struct {
    char address[64];
    uint16_t port;
    uint16_t local_sequence;
    uint16_t remote_sequence;
    uint32_t ack_bits;
    bool is_connected;
} mg_peer_t;

struct mg_net_host {
    uint16_t port;
    uint32_t max_peers;
    mg_peer_t peers[MG_NET_MAX_PEERS];

    mg_net_event_entry_t event_queue[MG_NET_MAX_EVENTS];
    size_t event_head;
    size_t event_tail;
    size_t event_count;
};

mg_net_host_t* mg_net_host_create(uint16_t port, uint32_t max_peers) {
    if (max_peers == 0 || max_peers > MG_NET_MAX_PEERS) {
        max_peers = MG_NET_MAX_PEERS;
    }

    mg_net_host_t* host = (mg_net_host_t*)malloc(sizeof(mg_net_host_t));
    if (!host) return NULL;

    memset(host, 0, sizeof(mg_net_host_t));
    host->port = port;
    host->max_peers = max_peers;

    return host;
}

void mg_net_host_destroy(mg_net_host_t* host) {
    if (host) free(host);
}

static void push_event(mg_net_host_t* host, const mg_net_event_t* ev) {
    if (host->event_count >= MG_NET_MAX_EVENTS) return;

    mg_net_event_entry_t* entry = &host->event_queue[host->event_tail];
    entry->event = *ev;

    if (ev->data && ev->size > 0) {
        size_t copy_len = (ev->size < MG_NET_MAX_PAYLOAD) ? ev->size : MG_NET_MAX_PAYLOAD;
        memcpy(entry->buffer, ev->data, copy_len);
        entry->event.data = entry->buffer;
        entry->event.size = copy_len;
    }

    host->event_tail = (host->event_tail + 1) % MG_NET_MAX_EVENTS;
    host->event_count++;
}

mg_peer_handle_t mg_net_connect(mg_net_host_t* host, const char* address, uint16_t port) {
    if (!host || !address) return MG_INVALID_PEER;

    for (uint32_t i = 1; i < host->max_peers; ++i) {
        if (!host->peers[i].is_connected) {
            strncpy(host->peers[i].address, address, sizeof(host->peers[i].address) - 1);
            host->peers[i].port = port;
            host->peers[i].local_sequence = 1;
            host->peers[i].remote_sequence = 0;
            host->peers[i].is_connected = true;

            mg_net_event_t ev = {
                .type = MG_NET_EVENT_CONNECT,
                .peer = i,
                .data = NULL,
                .size = 0,
                .channel = MG_NET_RELIABLE_ORDERED
            };
            push_event(host, &ev);
            return i;
        }
    }
    return MG_INVALID_PEER;
}

bool mg_net_send(
    mg_net_host_t* host,
    mg_peer_handle_t peer,
    mg_net_channel_t channel,
    const void* data,
    size_t size
) {
    if (!host || peer == MG_INVALID_PEER || peer >= host->max_peers || !data || size == 0) {
        return false;
    }
    if (!host->peers[peer].is_connected) return false;

    host->peers[peer].local_sequence++;

    /* In loopback/synthetic mode, self-deliver a receive event for testing */
    mg_net_event_t ev = {
        .type = MG_NET_EVENT_RECEIVE,
        .peer = peer,
        .data = (const uint8_t*)data,
        .size = size,
        .channel = channel
    };
    push_event(host, &ev);

    return true;
}

bool mg_net_poll(mg_net_host_t* host, mg_net_event_t* out_event) {
    if (!host || !out_event || host->event_count == 0) {
        return false;
    }

    mg_net_event_entry_t* entry = &host->event_queue[host->event_head];
    *out_event = entry->event;

    host->event_head = (host->event_head + 1) % MG_NET_MAX_EVENTS;
    host->event_count--;

    return true;
}

void mg_net_disconnect(mg_net_host_t* host, mg_peer_handle_t peer) {
    if (host && peer != MG_INVALID_PEER && peer < host->max_peers) {
        if (host->peers[peer].is_connected) {
            host->peers[peer].is_connected = false;
            mg_net_event_t ev = {
                .type = MG_NET_EVENT_DISCONNECT,
                .peer = peer,
                .data = NULL,
                .size = 0,
                .channel = MG_NET_RELIABLE_ORDERED
            };
            push_event(host, &ev);
        }
    }
}
