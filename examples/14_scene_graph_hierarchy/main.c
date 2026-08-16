/**
 * @file main.c
 * @brief Example 14: Scene Transform Node Graph Hierarchy Showcase.
 */
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 14: Scene Transform Node Graph Hierarchy Showcase ===\n");

    /* 1. Build Solar System Node Tree: Sun (Root) -> Earth (Child) -> Moon (Grandchild) */
    mg_scene_node_t* sun = mg_scene_node_create("Sun");
    mg_scene_node_t* earth = mg_scene_node_create("Earth");
    mg_scene_node_t* moon = mg_scene_node_create("Moon");

    assert(sun != NULL && earth != NULL && moon != NULL);

    sun->local_position = (mg_vec3_t){ 100.0f, 0.0f, 0.0f };
    earth->local_position = (mg_vec3_t){ 50.0f, 0.0f, 0.0f };
    moon->local_position = (mg_vec3_t){ 5.0f, 0.0f, 0.0f };

    bool r1 = mg_scene_node_add_child(sun, earth);
    bool r2 = mg_scene_node_add_child(earth, moon);
    assert(r1 && r2);

    /* 2. Propagate Transforms */
    mg_scene_node_update_transforms(sun, NULL);

    /* Verify Moon's world X translation component is 100 + 50 + 5 = 155 */
    /* In row-major Vulkan matrix layout: m[3] is translation X */
    float moon_world_x = moon->world_matrix.m[3];
    assert(moon_world_x > 154.9f && moon_world_x < 155.1f);
    printf("PASS: Hierarchy Transform Propagated (Moon World X = %.1f)\n", moon_world_x);

    /* 3. Destroy root and recursive tree */
    mg_scene_node_destroy(sun);
    printf("PASS: Scene Transform Node Graph Hierarchy destroyed cleanly\n");

    printf("PASS: Example 14: Scene Transform Node Graph Hierarchy completed successfully\n");
    return 0;
}
