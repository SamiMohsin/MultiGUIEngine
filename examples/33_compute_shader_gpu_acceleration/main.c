/**
 * @file main.c
 * @brief Example 33: Compute Shader Acceleration, Parallel Matrix Skinning & High-Throughput Particles.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define PARTICLE_COUNT 100000
#define VERTEX_COUNT   10000

int main(void) {
    printf("=== Example 33: Compute Shader GPU Acceleration & Parallel Pipeline ===\n");

    /* 1. High-Throughput Particle Physics Compute Dispatch (100,000 Particles) */
    mg_vec3_t* positions = (mg_vec3_t*)malloc(PARTICLE_COUNT * sizeof(mg_vec3_t));
    mg_vec3_t* velocities = (mg_vec3_t*)malloc(PARTICLE_COUNT * sizeof(mg_vec3_t));
    float* lifetimes = (float*)malloc(PARTICLE_COUNT * sizeof(float));
    assert(positions && velocities && lifetimes);

    for (size_t i = 0; i < PARTICLE_COUNT; ++i) {
        positions[i] = (mg_vec3_t){ 0.0f, 0.0f, 0.0f };
        velocities[i] = (mg_vec3_t){ (float)(i % 20) - 10.0f, 50.0f, (float)(i % 10) - 5.0f };
        lifetimes[i] = 2.0f;
    }

    mg_vec3_t gravity = { 0.0f, -9.81f, 0.0f };
    float dt = 0.016f; /* 60 FPS tick */

    mg_compute_particles_dispatch(positions, velocities, lifetimes, PARTICLE_COUNT, dt, &gravity);
    printf("PASS: Dispatched 100,000 Particle Physics Compute Pass (Particle #0 Pos: %.2f, %.2f, %.2f, Life: %.3f)\n",
           positions[0].x, positions[0].y, positions[0].z, lifetimes[0]);
    assert(positions[0].y > 0.0f);
    assert(lifetimes[0] < 2.0f);

    /* 2. Parallel Matrix Palette Skeletal Vertex Skinning Compute Dispatch (10,000 Vertices) */
    mg_vec3_t* bind_vertices = (mg_vec3_t*)malloc(VERTEX_COUNT * sizeof(mg_vec3_t));
    uint8_t* bone_indices = (uint8_t*)malloc(VERTEX_COUNT * 4 * sizeof(uint8_t));
    float* bone_weights = (float*)malloc(VERTEX_COUNT * 4 * sizeof(float));
    mg_vec3_t* skinned_vertices = (mg_vec3_t*)malloc(VERTEX_COUNT * sizeof(mg_vec3_t));
    assert(bind_vertices && bone_indices && bone_weights && skinned_vertices);

    for (size_t i = 0; i < VERTEX_COUNT; ++i) {
        bind_vertices[i] = (mg_vec3_t){ 0.0f, (float)i * 0.01f, 0.0f };
        bone_indices[i * 4 + 0] = 0;
        bone_indices[i * 4 + 1] = 1;
        bone_indices[i * 4 + 2] = 0;
        bone_indices[i * 4 + 3] = 0;
        bone_weights[i * 4 + 0] = 0.5f;
        bone_weights[i * 4 + 1] = 0.5f;
        bone_weights[i * 4 + 2] = 0.0f;
        bone_weights[i * 4 + 3] = 0.0f;
    }

    mg_mat4_t bone_palette[2];
    mg_mat4_identity(&bone_palette[0]);
    mg_vec3_t offset = { 10.0f, 0.0f, 0.0f };
    mg_mat4_translation(&bone_palette[1], &offset);

    mg_compute_skinning_dispatch(bind_vertices, bone_indices, bone_weights, VERTEX_COUNT, bone_palette, skinned_vertices);
    printf("PASS: Dispatched 10,000 Vertex Matrix Palette Skinning Compute Pass (Vertex #100 Skinned: %.2f, %.2f, %.2f)\n",
           skinned_vertices[100].x, skinned_vertices[100].y, skinned_vertices[100].z);
    /* 50% identity (0) + 50% offset +10 (5.0) -> x = 5.0 */
    assert(fabsf(skinned_vertices[100].x - 5.0f) < 0.001f);

    /* Free buffers */
    free(positions);
    free(velocities);
    free(lifetimes);
    free(bind_vertices);
    free(bone_indices);
    free(bone_weights);
    free(skinned_vertices);

    printf("PASS: Example 33: Compute Shader Acceleration completed successfully\n");
    return 0;
}
