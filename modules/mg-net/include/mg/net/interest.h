/**
 * @file interest.h
 * @brief MultiGUIEngine Spatial Interest Management & Replication Filtering Engine.
 */
#ifndef MG_NET_INTEREST_H
#define MG_NET_INTEREST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_NET_MAX_SPATIAL_ENTITIES 16384
#define MG_NET_GRID_CELLS_X 64
#define MG_NET_GRID_CELLS_Z 64

typedef struct mg_net_entity_pos {
    uint32_t entity_id;
    float x;
    float z;
    bool active;
} mg_net_entity_pos_t;

typedef struct mg_net_interest {
    float cell_size;
    float world_min_x;
    float world_min_z;
    float world_max_x;
    float world_max_z;
    mg_net_entity_pos_t entities[MG_NET_MAX_SPATIAL_ENTITIES];
    uint32_t active_count;
} mg_net_interest_t;

/**
 * @brief Initializes a spatial interest manager over a 2D world domain.
 */
void mg_net_interest_init(
    mg_net_interest_t* mgr,
    float cell_size,
    float world_min_x,
    float world_min_z,
    float world_max_x,
    float world_max_z
);

/**
 * @brief Registers or updates an entity's 2D position for spatial interest management.
 */
void mg_net_interest_set_entity(
    mg_net_interest_t* mgr,
    uint32_t entity_id,
    float x,
    float z
);

/**
 * @brief Queries all entities within a client's interest radius.
 * @return Total number of visible entities found (capped at max_results).
 */
size_t mg_net_interest_query_for_client(
    const mg_net_interest_t* mgr,
    float client_x,
    float client_z,
    float view_radius,
    uint32_t* out_entities,
    size_t max_results
);

#ifdef __cplusplus
}
#endif

#endif /* MG_NET_INTEREST_H */
