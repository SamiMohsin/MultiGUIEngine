/**
 * @file main.c
 * @brief Example 36: Spatial Interest Management & Grid Replication Filtering.
 */
#include "mg/net/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define TOTAL_ENTITIES 10000
#define MAX_RELEVANT   1000

int main(void) {
    printf("=== Example 36: Spatial Interest Management & Replication Filtering ===\n");

    /* 1. Initialize Spatial Interest Grid (2000m x 2000m World) */
    mg_net_interest_t interest_mgr;
    mg_net_interest_init(&interest_mgr, 50.0f, -1000.0f, -1000.0f, 1000.0f, 1000.0f);

    /* 2. Populate 10,000 Entities across the MMO World */
    for (uint32_t i = 0; i < TOTAL_ENTITIES; ++i) {
        float x = (float)(i % 100) * 20.0f - 990.0f;
        float z = (float)(i / 100) * 20.0f - 990.0f;
        mg_net_interest_set_entity(&interest_mgr, i, x, z);
    }
    printf("PASS: Registered %u Entities into Network Spatial Interest Manager (Active Count: %u)\n",
           TOTAL_ENTITIES, interest_mgr.active_count);
    assert(interest_mgr.active_count == TOTAL_ENTITIES);

    /* 3. Query Relevant Entities for Client at (100.0, 200.0) with View Radius = 150.0m */
    float client_x = 100.0f;
    float client_z = 200.0f;
    float view_radius = 150.0f;

    uint32_t relevant_entities[MAX_RELEVANT];
    size_t count = mg_net_interest_query_for_client(&interest_mgr, client_x, client_z, view_radius,
                                                    relevant_entities, MAX_RELEVANT);

    printf("PASS: Spatial Relevance Query found %zu entities within %.1fm view radius\n",
           count, view_radius);
    assert(count > 0 && count < TOTAL_ENTITIES);

    /* Bandwidth reduction calculation */
    float bandwidth_saved_pct = (1.0f - ((float)count / (float)TOTAL_ENTITIES)) * 100.0f;
    printf("PASS: Network Replication Filter eliminated %u out-of-range updates (Bandwidth Reduction: %.2f%%)\n",
           (uint32_t)(TOTAL_ENTITIES - count), bandwidth_saved_pct);
    assert(bandwidth_saved_pct > 95.0f);

    /* 4. Validate Distance Fidelity of Filtered Entities */
    float rad_sq = view_radius * view_radius;
    for (size_t r = 0; r < count; ++r) {
        uint32_t id = relevant_entities[r];
        float dx = interest_mgr.entities[id].x - client_x;
        float dz = interest_mgr.entities[id].z - client_z;
        assert(dx * dx + dz * dz <= rad_sq + 0.001f);
    }

    printf("PASS: Example 36: Spatial Interest Management completed successfully\n");
    return 0;
}
