/**
 * @file main.c
 * @brief mg-profiler: MultiGUIEngine High-Resolution Frame & Stage Profiler Tool.
 */
#include "mg/pal/time.h"
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>

#define COMP_PROFILER_POS 0

typedef struct {
    float x, y, z;
} profiler_pos_t;

static void benchmark_update_system(mg_world_t* world, float dt, void* ctx) {
    (void)dt; (void)ctx;
    for (uint32_t i = 1; i < 512; ++i) {
        if (mg_entity_is_alive(world, i)) {
            profiler_pos_t* p = (profiler_pos_t*)mg_component_get(world, i, COMP_PROFILER_POS);
            if (p) {
                p->x += 0.01f;
            }
        }
    }
}

int main(void) {
    printf("=== MultiGUIEngine High-Resolution Stage Profiler (mg-profiler) ===\n");

    mg_world_t* world = mg_world_create();
    if (!world) return 1;

    mg_component_register(world, COMP_PROFILER_POS, sizeof(profiler_pos_t));
    mg_system_register(world, benchmark_update_system, NULL, MG_STAGE_UPDATE);

    /* Populate 500 entities */
    for (int i = 0; i < 500; ++i) {
        mg_entity_t e = mg_entity_create(world);
        profiler_pos_t* p = (profiler_pos_t*)mg_component_add(world, e, COMP_PROFILER_POS);
        p->x = (float)i;
    }

    const int total_frames = 1000;
    uint64_t start_bench_ns = mg_time_now_ns();

    for (int f = 0; f < total_frames; ++f) {
        mg_world_tick(world, 0.016f);
    }

    uint64_t total_ns = mg_time_now_ns() - start_bench_ns;
    double total_ms = (double)total_ns / 1000000.0;
    double avg_frame_us = (double)total_ns / (double)total_frames / 1000.0;
    double fps = (double)total_frames / (total_ms / 1000.0);

    printf("Simulated %d frames over 500 ECS entities in %.2f ms\n", total_frames, total_ms);
    printf("Average Tick Latency: %.2f µs per frame (~%.0f FPS equivalent)\n", avg_frame_us, fps);

    mg_world_destroy(world);
    printf("PASS: mg-profiler executed successfully\n");
    return 0;
}
