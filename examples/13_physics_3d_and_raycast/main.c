/**
 * @file main.c
 * @brief Example 13: 3D Physics Primitive Collisions and Raycast Queries.
 */
#include "mg/physics/physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 13: 3D Physics Primitive Collisions and Raycast Queries ===\n");

    /* 1. Test 3D Sphere - Sphere Intersection */
    mg_sphere_t s1 = { .center = { 0.0f, 0.0f, 0.0f }, .radius = 2.0f };
    mg_sphere_t s2 = { .center = { 3.0f, 0.0f, 0.0f }, .radius = 2.0f };
    mg_sphere_t s3 = { .center = { 10.0f, 0.0f, 0.0f }, .radius = 1.0f };

    assert(mg_physics_intersect_sphere_sphere(&s1, &s2) == true);
    assert(mg_physics_intersect_sphere_sphere(&s1, &s3) == false);
    printf("PASS: 3D Sphere-Sphere Intersections verified\n");

    /* 2. Test 3D Sphere - AABB Intersection */
    mg_aabb3d_t box = { .min = { -1.0f, -1.0f, -1.0f }, .max = { 1.0f, 1.0f, 1.0f } };
    assert(mg_physics_intersect_sphere_aabb(&s1, &box) == true);
    assert(mg_physics_intersect_sphere_aabb(&s3, &box) == false);
    printf("PASS: 3D Sphere-AABB Intersections verified\n");

    /* 3. Test 3D Raycast against Sphere */
    mg_ray3d_t ray = { .origin = { 0.0f, 0.0f, -10.0f }, .direction = { 0.0f, 0.0f, 1.0f } };
    mg_raycast_hit3d_t hit;
    bool did_hit = mg_physics_raycast_sphere(&ray, &s1, &hit);
    assert(did_hit == true);
    assert(hit.distance > 7.9f && hit.distance < 8.1f);
    printf("PASS: 3D Raycast Sphere Hit at distance: %.2f (Hit point z: %.2f)\n", hit.distance, hit.point.z);

    /* 4. Test 3D Raycast against AABB */
    bool box_hit = mg_physics_raycast_aabb(&ray, &box, &hit);
    assert(box_hit == true);
    printf("PASS: 3D Raycast AABB Hit at distance: %.2f (Hit point z: %.2f)\n", hit.distance, hit.point.z);

    printf("PASS: Example 13: 3D Physics Primitive Collisions and Raycast completed successfully\n");
    return 0;
}
