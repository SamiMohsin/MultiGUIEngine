/**
 * @file replication.h
 * @brief MultiGUIEngine ECS Network Entity Replication and Delta Compression.
 */
#ifndef MG_NET_REPLICATION_H
#define MG_NET_REPLICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_net_entity_snapshot {
    uint32_t net_id;
    float x, y, z;
    float vx, vy, vz;
    uint32_t health;
    uint32_t state_flags;
} mg_net_entity_snapshot_t;

typedef struct mg_net_replicator mg_net_replicator_t;

mg_net_replicator_t* mg_net_replicator_create(bool is_server);
void mg_net_replicator_destroy(mg_net_replicator_t* rep);

/**
 * @brief Server: serializes entity snapshot delta into output buffer.
 */
size_t mg_net_replicate_serialize(
    mg_net_replicator_t* rep,
    const mg_net_entity_snapshot_t* snapshots,
    uint32_t count,
    uint8_t* out_buffer,
    size_t buffer_capacity
);

/**
 * @brief Client: deserializes entity snapshots from packet buffer.
 */
uint32_t mg_net_replicate_deserialize(
    mg_net_replicator_t* rep,
    const uint8_t* in_buffer,
    size_t buffer_size,
    mg_net_entity_snapshot_t* out_snapshots,
    uint32_t max_snapshots
);

#ifdef __cplusplus
}
#endif

#endif /* MG_NET_REPLICATION_H */
