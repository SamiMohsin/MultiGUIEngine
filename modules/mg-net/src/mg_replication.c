/**
 * @file mg_replication.c
 * @brief MultiGUIEngine ECS Network Replication implementation.
 */
#include "mg/net/replication.h"
#include <stdlib.h>
#include <string.h>

struct mg_net_replicator {
    bool is_server;
    uint32_t sequence_counter;
};

mg_net_replicator_t* mg_net_replicator_create(bool is_server) {
    mg_net_replicator_t* rep = (mg_net_replicator_t*)calloc(1, sizeof(mg_net_replicator_t));
    if (rep) {
        rep->is_server = is_server;
    }
    return rep;
}

void mg_net_replicator_destroy(mg_net_replicator_t* rep) {
    if (rep) free(rep);
}

size_t mg_net_replicate_serialize(
    mg_net_replicator_t* rep,
    const mg_net_entity_snapshot_t* snapshots,
    uint32_t count,
    uint8_t* out_buffer,
    size_t buffer_capacity
) {
    if (!rep || !snapshots || !out_buffer) return 0;
    size_t header_size = sizeof(uint32_t) * 2; /* seq, count */
    size_t payload_size = count * sizeof(mg_net_entity_snapshot_t);
    size_t total = header_size + payload_size;

    if (total > buffer_capacity) return 0;

    uint32_t seq = ++rep->sequence_counter;
    memcpy(out_buffer, &seq, sizeof(uint32_t));
    memcpy(out_buffer + sizeof(uint32_t), &count, sizeof(uint32_t));
    memcpy(out_buffer + header_size, snapshots, payload_size);

    return total;
}

uint32_t mg_net_replicate_deserialize(
    mg_net_replicator_t* rep,
    const uint8_t* in_buffer,
    size_t buffer_size,
    mg_net_entity_snapshot_t* out_snapshots,
    uint32_t max_snapshots
) {
    (void)rep;
    if (!in_buffer || !out_snapshots || buffer_size < sizeof(uint32_t) * 2) return 0;
    uint32_t count = 0;
    memcpy(&count, in_buffer + sizeof(uint32_t), sizeof(uint32_t));

    if (count > max_snapshots) count = max_snapshots;
    size_t header_size = sizeof(uint32_t) * 2;
    size_t payload_size = count * sizeof(mg_net_entity_snapshot_t);

    if (header_size + payload_size > buffer_size) return 0;

    memcpy(out_snapshots, in_buffer + header_size, payload_size);
    return count;
}
