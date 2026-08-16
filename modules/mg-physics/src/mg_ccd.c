/**
 * @file mg_ccd.c
 * @brief MultiGUIEngine 3D Continuous Collision Detection (CCD) & Swept Intersection Tests implementation.
 */
#include "mg/physics/ccd.h"
#include <math.h>

mg_ccd_hit_t mg_ccd_swept_sphere_plane(
    const mg_vec3_t* start_pos,
    const mg_vec3_t* velocity,
    float radius,
    const mg_physics_plane_t* plane
) {
    mg_ccd_hit_t hit = {
        .has_hit = false,
        .time_of_impact = 1.0f,
        .hit_point = { 0.0f, 0.0f, 0.0f },
        .hit_normal = { 0.0f, 0.0f, 0.0f }
    };

    if (!start_pos || !velocity || !plane) return hit;

    float n_dot_v = plane->normal.x * velocity->x + plane->normal.y * velocity->y + plane->normal.z * velocity->z;
    float dist0 = (start_pos->x * plane->normal.x + start_pos->y * plane->normal.y + start_pos->z * plane->normal.z) - plane->distance;

    /* If not moving towards the plane */
    if (dist0 > 0.0f && n_dot_v >= 0.0f) return hit;
    if (dist0 < 0.0f && n_dot_v <= 0.0f) return hit;

    float effective_dist = dist0 > 0.0f ? (dist0 - radius) : (dist0 + radius);
    float t = -effective_dist / n_dot_v;

    if (t >= 0.0f && t <= 1.0f) {
        hit.has_hit = true;
        hit.time_of_impact = t;
        hit.hit_normal = plane->normal;
        hit.hit_point = (mg_vec3_t){
            (start_pos->x + velocity->x * t) - plane->normal.x * (dist0 > 0.0f ? radius : -radius),
            (start_pos->y + velocity->y * t) - plane->normal.y * (dist0 > 0.0f ? radius : -radius),
            (start_pos->z + velocity->z * t) - plane->normal.z * (dist0 > 0.0f ? radius : -radius)
        };
    }

    return hit;
}

mg_ccd_hit_t mg_ccd_swept_sphere_sphere(
    const mg_vec3_t* pos_a,
    const mg_vec3_t* vel_a,
    float radius_a,
    const mg_vec3_t* pos_b,
    const mg_vec3_t* vel_b,
    float radius_b
) {
    mg_ccd_hit_t hit = {
        .has_hit = false,
        .time_of_impact = 1.0f,
        .hit_point = { 0.0f, 0.0f, 0.0f },
        .hit_normal = { 0.0f, 0.0f, 0.0f }
    };

    if (!pos_a || !vel_a || !pos_b || !vel_b) return hit;

    float r = radius_a + radius_b;
    float dx = pos_a->x - pos_b->x;
    float dy = pos_a->y - pos_b->y;
    float dz = pos_a->z - pos_b->z;

    float dvx = vel_a->x - vel_b->x;
    float dvy = vel_a->y - vel_b->y;
    float dvz = vel_a->z - vel_b->z;

    float c = (dx * dx + dy * dy + dz * dz) - (r * r);
    if (c <= 0.0f) {
        /* Already overlapping at t = 0 */
        hit.has_hit = true;
        hit.time_of_impact = 0.0f;
        hit.hit_point = *pos_a;
        return hit;
    }

    float a = dvx * dvx + dvy * dvy + dvz * dvz;
    if (a < 0.0001f) return hit; /* Not moving relative to each other */

    float b = 2.0f * (dx * dvx + dy * dvy + dz * dvz);
    if (b >= 0.0f) return hit; /* Moving away from each other */

    float d = b * b - 4.0f * a * c;
    if (d < 0.0f) return hit; /* No intersection along trajectory */

    float t = (-b - sqrtf(d)) / (2.0f * a);
    if (t >= 0.0f && t <= 1.0f) {
        hit.has_hit = true;
        hit.time_of_impact = t;
        mg_vec3_t center_a = { pos_a->x + vel_a->x * t, pos_a->y + vel_a->y * t, pos_a->z + vel_a->z * t };
        mg_vec3_t center_b = { pos_b->x + vel_b->x * t, pos_b->y + vel_b->y * t, pos_b->z + vel_b->z * t };

        float nx = center_a.x - center_b.x;
        float ny = center_a.y - center_b.y;
        float nz = center_a.z - center_b.z;
        float nlen = sqrtf(nx * nx + ny * ny + nz * nz);
        if (nlen > 0.0f) { nx /= nlen; ny /= nlen; nz /= nlen; }

        hit.hit_normal = (mg_vec3_t){ nx, ny, nz };
        hit.hit_point = (mg_vec3_t){ center_a.x - nx * radius_a, center_a.y - ny * radius_a, center_a.z - nz * radius_a };
    }

    return hit;
}
