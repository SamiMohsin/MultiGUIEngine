/**
 * @file main.c
 * @brief Example 22: 3D Continuous Collision Detection (CCD) & Tunneling Prevention.
 */
#include "mg/physics/physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 22: 3D Continuous Collision Detection (CCD) & Swept Intersection ===\n");

    /* 1. Thin Obstacle Wall at x = 50.0 with normal (-1, 0, 0) */
    mg_physics_plane_t wall = {
        .normal = { -1.0f, 0.0f, 0.0f },
        .distance = -50.0f
    };

    /* 2. Fast Bullet Projectile (radius = 0.5m) moving from x = 10.0 to x = 90.0 in 1 frame (vel_x = +80.0) */
    mg_vec3_t bullet_start = { 10.0f, 0.0f, 0.0f };
    mg_vec3_t bullet_vel = { 80.0f, 0.0f, 0.0f };
    float bullet_radius = 0.5f;

    /* Discrete collision test (evaluating start vs end points) completely misses the thin wall! */
    mg_vec3_t bullet_end = { bullet_start.x + bullet_vel.x, bullet_start.y, bullet_start.z };
    printf("Discrete Check: Start x=%.1f (outside), End x=%.1f (outside) -> TUNNELING DEFECT!\n",
           bullet_start.x, bullet_end.x);

    /* 3. Continuous Collision Detection (CCD) Swept Test */
    mg_ccd_hit_t hit = mg_ccd_swept_sphere_plane(&bullet_start, &bullet_vel, bullet_radius, &wall);
    assert(hit.has_hit == true);
    assert(hit.time_of_impact > 0.0f && hit.time_of_impact < 1.0f);
    printf("PASS: CCD Swept Wall Hit Detected at TOI: %.3f (Exact hit x: %.2f)\n",
           hit.time_of_impact, hit.hit_point.x);

    /* 4. Swept Moving Sphere vs Moving Sphere Collision */
    mg_vec3_t sphere_a_pos = { -10.0f, 0.0f, 0.0f };
    mg_vec3_t sphere_a_vel = {  20.0f, 0.0f, 0.0f }; /* Moving right */
    float radius_a = 1.0f;

    mg_vec3_t sphere_b_pos = {  10.0f, 0.0f, 0.0f };
    mg_vec3_t sphere_b_vel = { -20.0f, 0.0f, 0.0f }; /* Moving left */
    float radius_b = 1.0f;

    mg_ccd_hit_t sphere_hit = mg_ccd_swept_sphere_sphere(
        &sphere_a_pos, &sphere_a_vel, radius_a,
        &sphere_b_pos, &sphere_b_vel, radius_b
    );

    assert(sphere_hit.has_hit == true);
    printf("PASS: CCD Moving Sphere-Sphere Collision at TOI: %.3f\n", sphere_hit.time_of_impact);
    assert(sphere_hit.time_of_impact == 0.45f); /* dist = 20 - 2 = 18, rel_vel = 40, t = 18/40 = 0.45 */

    printf("PASS: Example 22: 3D Continuous Collision Detection completed successfully\n");
    return 0;
}
