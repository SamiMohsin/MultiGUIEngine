/**
 * @file gameplay.h
 * @brief MultiGUIEngine Gameplay Subsystems: Hierarchical FSM, A* Pathfinding, Particle Emitters.
 */
#ifndef MG_SCENE_GAMEPLAY_H
#define MG_SCENE_GAMEPLAY_H

#include "mg/math/vec4.h"
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/* --- 1. Finite State Machine (FSM) --- */

typedef void (*mg_fsm_state_fn)(void* ctx, float dt);

typedef struct mg_fsm_state_desc {
    uint32_t state_id;
    mg_fsm_state_fn on_enter;
    mg_fsm_state_fn on_update;
    mg_fsm_state_fn on_exit;
} mg_fsm_state_desc_t;

typedef struct mg_fsm mg_fsm_t;

mg_fsm_t* mg_fsm_create(uint32_t initial_state);
void mg_fsm_destroy(mg_fsm_t* fsm);
bool mg_fsm_add_state(mg_fsm_t* fsm, const mg_fsm_state_desc_t* desc);
void mg_fsm_transition(mg_fsm_t* fsm, uint32_t new_state, void* ctx);
void mg_fsm_tick(mg_fsm_t* fsm, float dt, void* ctx);
uint32_t mg_fsm_get_current_state(const mg_fsm_t* fsm);

/* --- 2. A* Grid Pathfinding --- */

#define MG_MAX_PATH_NODES 128

typedef struct mg_grid_node {
    uint32_t x;
    uint32_t y;
} mg_grid_node_t;

typedef struct mg_path {
    mg_grid_node_t nodes[MG_MAX_PATH_NODES];
    uint32_t node_count;
    bool path_found;
} mg_path_t;

/**
 * @brief Computes shortest path on a 2D occupancy grid (0 = traversable, 1 = obstacle).
 */
mg_path_t mg_pathfind_astar(
    const uint8_t* grid,
    uint32_t grid_width,
    uint32_t grid_height,
    uint32_t start_x,
    uint32_t start_y,
    uint32_t target_x,
    uint32_t target_y
);

/* --- 3. Particle System --- */

typedef struct mg_particle {
    float x, y;
    float vx, vy;
    float life;
    float max_life;
    mg_vec4_t color;
    bool active;
} mg_particle_t;

typedef struct mg_particle_emitter mg_particle_emitter_t;

mg_particle_emitter_t* mg_particle_emitter_create(uint32_t max_particles);
void mg_particle_emitter_destroy(mg_particle_emitter_t* emitter);
void mg_particle_emitter_burst(
    mg_particle_emitter_t* emitter,
    float x, float y,
    uint32_t count,
    float speed,
    const mg_vec4_t* color
);
void mg_particle_emitter_tick(mg_particle_emitter_t* emitter, float dt, float gravity_y);
uint32_t mg_particle_emitter_get_active_count(const mg_particle_emitter_t* emitter);
const mg_particle_t* mg_particle_emitter_get_particles(const mg_particle_emitter_t* emitter);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_GAMEPLAY_H */
