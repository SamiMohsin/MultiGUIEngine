/**
 * @file net.h
 * @brief MultiGUIEngine Custom Reliable UDP Network Transport Subsystem.
 */
#ifndef MG_NET_NET_H
#define MG_NET_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t mg_peer_handle_t;
#define MG_INVALID_PEER 0

typedef enum mg_net_channel {
    MG_NET_UNRELIABLE = 0,
    MG_NET_RELIABLE_ORDERED
} mg_net_channel_t;

typedef enum mg_net_event_type {
    MG_NET_EVENT_NONE = 0,
    MG_NET_EVENT_CONNECT,
    MG_NET_EVENT_DISCONNECT,
    MG_NET_EVENT_RECEIVE
} mg_net_event_type_t;

typedef struct mg_net_event {
    mg_net_event_type_t type;
    mg_peer_handle_t peer;
    const uint8_t* data;
    size_t size;
    mg_net_channel_t channel;
} mg_net_event_t;

typedef struct mg_net_host mg_net_host_t;

/**
 * @brief Creates a local network host instance (server or client).
 * @param port Port to bind to (0 for ephemeral client port).
 * @param max_peers Maximum concurrent peer connections.
 */
mg_net_host_t* mg_net_host_create(uint16_t port, uint32_t max_peers);

/**
 * @brief Destroys a network host.
 */
void mg_net_host_destroy(mg_net_host_t* host);

/**
 * @brief Connects to a remote host endpoint.
 */
mg_peer_handle_t mg_net_connect(mg_net_host_t* host, const char* address, uint16_t port);

/**
 * @brief Sends a packet payload to a connected peer.
 */
bool mg_net_send(
    mg_net_host_t* host,
    mg_peer_handle_t peer,
    mg_net_channel_t channel,
    const void* data,
    size_t size
);

/**
 * @brief Polls incoming network events (non-blocking).
 * @param host Pointer to host.
 * @param out_event Output event structure.
 * @return True if an event was dequeued, false if queue is empty.
 */
bool mg_net_poll(mg_net_host_t* host, mg_net_event_t* out_event);

/**
 * @brief Disconnects a peer.
 */
void mg_net_disconnect(mg_net_host_t* host, mg_peer_handle_t peer);

#ifdef __cplusplus
}
#endif

#include "mg/net/replication.h"

#endif /* MG_NET_NET_H */
