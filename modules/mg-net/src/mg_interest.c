/**
 * @file mg_interest.c
 * @brief MultiGUIEngine Spatial Interest Management & Replication Filtering Engine implementation.
 */
#include "mg/net/interest.h"
#include <string.h>

void mg_net_interest_init(
    mg_net_interest_t* mgr,
    float cell_size,
    float world_min_x,
    float world_min_z,
    float world_max_x,
    float world_max_z
) {
    if (!mgr) return;
    memset(mgr, 0, sizeof(mg_net_interest_t));
    mgr->cell_size = cell_size > 0.0f ? cell_size : 50.0f;
    mgr->world_min_x = world_min_x;
    mgr->world_min_z = world_min_z;
    mgr->world_max_x = world_max_x;
    mgr->world_max_z = world_max_z;
}

void mg_net_interest_set_entity(
    mg_net_interest_t* mgr,
    uint32_t entity_id,
    float x,
    float z
) {
    if (!mgr || entity_id >= MG_NET_MAX_SPATIAL_ENTITIES) return;

    if (!mgr->entities[entity_id].active) {
        mgr->active_count++;
    }

    mgr->entities[entity_id].entity_id = entity_id;
    mgr->entities[entity_id].x = x;
    mgr->entities[entity_id].z = z;
    mgr->entities[entity_id].active = true;
}

size_t mg_net_interest_query_for_client(
    const mg_net_interest_t* mgr,
    float client_x,
    float client_z,
    float view_radius,
    uint32_t* out_entities,
    size_t max_results
) {
    if (!mgr || !out_entities || max_results == 0 || view_radius <= 0.0f) {
        return 0;
    }

    float rad_sq = view_radius * view_radius;
    size_t count = 0;

    for (uint32_t i = 0; i < MG_NET_MAX_SPATIAL_ENTITIES; ++i) {
        if (mgr->entities[i].active) {
            float dx = mgr->entities[i].x - client_x;
            float dz = mgr->entities[i].z - client_z;
            float dist_sq = dx * dx + dz * dz;

            if (dist_sq <= rad_sq) {
                out_entities[count++] = mgr->entities[i].entity_id;
                if (count >= max_results) {
                    break;
                }
            }
        }
    }

    return count;
}
