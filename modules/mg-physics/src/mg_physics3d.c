/**
 * @file mg_physics3d.c
 * @brief MultiGUIEngine 3D Physics Primitive Collisions and Raycast implementation.
 */
#include "mg/physics/physics3d.h"
#include <math.h>

static inline float mg_clampf(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

bool mg_physics_intersect_sphere_sphere(const mg_sphere_t* a, const mg_sphere_t* b) {
    if (!a || !b) return false;
    float dx = a->center.x - b->center.x;
    float dy = a->center.y - b->center.y;
    float dz = a->center.z - b->center.z;
    float dist_sq = dx * dx + dy * dy + dz * dz;
    float rad_sum = a->radius + b->radius;
    return dist_sq <= (rad_sum * rad_sum);
}

bool mg_physics_intersect_sphere_aabb(const mg_sphere_t* s, const mg_aabb3d_t* b) {
    if (!s || !b) return false;
    float cx = mg_clampf(s->center.x, b->min.x, b->max.x);
    float cy = mg_clampf(s->center.y, b->min.y, b->max.y);
    float cz = mg_clampf(s->center.z, b->min.z, b->max.z);

    float dx = s->center.x - cx;
    float dy = s->center.y - cy;
    float dz = s->center.z - cz;
    return (dx * dx + dy * dy + dz * dz) <= (s->radius * s->radius);
}

bool mg_physics_intersect_aabb_aabb(const mg_aabb3d_t* a, const mg_aabb3d_t* b) {
    if (!a || !b) return false;
    return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
           (a->min.y <= b->max.y && a->max.y >= b->min.y) &&
           (a->min.z <= b->max.z && a->max.z >= b->min.z);
}

bool mg_physics_raycast_sphere(const mg_ray3d_t* ray, const mg_sphere_t* sphere, mg_raycast_hit3d_t* out_hit) {
    if (!ray || !sphere) return false;

    float oc_x = ray->origin.x - sphere->center.x;
    float oc_y = ray->origin.y - sphere->center.y;
    float oc_z = ray->origin.z - sphere->center.z;

    float a = ray->direction.x * ray->direction.x + ray->direction.y * ray->direction.y + ray->direction.z * ray->direction.z;
    float b = 2.0f * (oc_x * ray->direction.x + oc_y * ray->direction.y + oc_z * ray->direction.z);
    float c = (oc_x * oc_x + oc_y * oc_y + oc_z * oc_z) - (sphere->radius * sphere->radius);

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) return false;

    float t = (-b - sqrtf(discriminant)) / (2.0f * a);
    if (t < 0.0f) {
        t = (-b + sqrtf(discriminant)) / (2.0f * a);
        if (t < 0.0f) return false;
    }

    if (out_hit) {
        out_hit->hit = true;
        out_hit->distance = t;
        out_hit->point.x = ray->origin.x + ray->direction.x * t;
        out_hit->point.y = ray->origin.y + ray->direction.y * t;
        out_hit->point.z = ray->origin.z + ray->direction.z * t;
        out_hit->normal.x = (out_hit->point.x - sphere->center.x) / sphere->radius;
        out_hit->normal.y = (out_hit->point.y - sphere->center.y) / sphere->radius;
        out_hit->normal.z = (out_hit->point.z - sphere->center.z) / sphere->radius;
    }

    return true;
}

bool mg_physics_raycast_aabb(const mg_ray3d_t* ray, const mg_aabb3d_t* aabb, mg_raycast_hit3d_t* out_hit) {
    if (!ray || !aabb) return false;

    float tmin = 0.0f;
    float tmax = 1e30f;

    /* X slab */
    if (fabsf(ray->direction.x) > 1e-6f) {
        float t1 = (aabb->min.x - ray->origin.x) / ray->direction.x;
        float t2 = (aabb->max.x - ray->origin.x) / ray->direction.x;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }

    /* Y slab */
    if (fabsf(ray->direction.y) > 1e-6f) {
        float t1 = (aabb->min.y - ray->origin.y) / ray->direction.y;
        float t2 = (aabb->max.y - ray->origin.y) / ray->direction.y;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }

    /* Z slab */
    if (fabsf(ray->direction.z) > 1e-6f) {
        float t1 = (aabb->min.z - ray->origin.z) / ray->direction.z;
        float t2 = (aabb->max.z - ray->origin.z) / ray->direction.z;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }

    if (out_hit) {
        out_hit->hit = true;
        out_hit->distance = tmin;
        out_hit->point.x = ray->origin.x + ray->direction.x * tmin;
        out_hit->point.y = ray->origin.y + ray->direction.y * tmin;
        out_hit->point.z = ray->origin.z + ray->direction.z * tmin;
        out_hit->normal = (mg_vec3_t){ 0.0f, 1.0f, 0.0f };
    }

    return true;
}
