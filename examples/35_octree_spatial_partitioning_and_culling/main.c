/**
 * @file main.c
 * @brief Example 35: 3D Spatial Octree Partitioning, Range Queries & Frustum Culling.
 */
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define ENTITY_COUNT 10000
#define MAX_RESULTS  1000

int main(void) {
    printf("=== Example 35: 3D Spatial Octree Partitioning & Culling Engine ===\n");

    /* 1. Initialize Spatial Octree across 1000m x 1000m x 1000m World */
    mg_octree_t octree;
    mg_vec3_t world_min = { -500.0f, -500.0f, -500.0f };
    mg_vec3_t world_max = {  500.0f,  500.0f,  500.0f };
    mg_octree_init(&octree, &world_min, &world_max, 6);

    /* 2. Populate 10,000 Spatial Entities */
    for (uint32_t i = 0; i < ENTITY_COUNT; ++i) {
        float x = (float)(i % 100) * 10.0f - 495.0f;
        float y = (float)((i / 100) % 10) * 100.0f - 450.0f;
        float z = (float)(i / 1000) * 100.0f - 450.0f;

        mg_vec3_t obj_min = { x - 1.0f, y - 1.0f, z - 1.0f };
        mg_vec3_t obj_max = { x + 1.0f, y + 1.0f, z + 1.0f };

        bool ok = mg_octree_insert(&octree, i + 1, &obj_min, &obj_max);
        assert(ok);
    }
    printf("PASS: Inserted %u Entities into 3D Spatial Octree (Total Registered: %u)\n",
           ENTITY_COUNT, octree.total_elements);
    assert(octree.total_elements == ENTITY_COUNT);

    /* 3. Perform Fast Range Query for Frustum / Area (Center Sector [-50..50]^3) */
    mg_vec3_t query_min = { -50.0f, -50.0f, -50.0f };
    mg_vec3_t query_max = {  50.0f,  50.0f,  50.0f };

    uint32_t results[MAX_RESULTS];
    size_t found = mg_octree_query_range(&octree, &query_min, &query_max, results, MAX_RESULTS);

    printf("PASS: Octree Range Query found %zu visible entities in target bounding volume\n", found);
    assert(found > 0 && found <= MAX_RESULTS);

    /* 4. Validate Query Accuracy against Ground Truth Linear Bounds */
    for (size_t r = 0; r < found; ++r) {
        uint32_t id = results[r] - 1;
        float x = (float)(id % 100) * 10.0f - 495.0f;
        float y = (float)((id / 100) % 10) * 100.0f - 450.0f;
        float z = (float)(id / 1000) * 100.0f - 450.0f;

        /* Object bounds must overlap with [-50..50]^3 */
        assert(x + 1.0f >= -50.0f && x - 1.0f <= 50.0f);
        assert(y + 1.0f >= -50.0f && y - 1.0f <= 50.0f);
        assert(z + 1.0f >= -50.0f && z - 1.0f <= 50.0f);
    }

    /* 5. Free Octree Memory */
    mg_octree_free(&octree);
    assert(octree.root == NULL);

    printf("PASS: Example 35: 3D Spatial Octree Partitioning & Culling completed successfully\n");
    return 0;
}
