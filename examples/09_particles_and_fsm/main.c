/**
 * @file main.c
 * @brief Example 09: Particles, A* Pathfinding, and Finite State Machine (FSM).
 */
#include "mg/render/render.h"
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STATE_IDLE 0
#define STATE_PATROL 1
#define STATE_ATTACK 2

typedef struct {
    int attack_counter;
} agent_context_t;

static void on_attack_enter(void* ctx, float dt) {
    (void)dt;
    agent_context_t* a = (agent_context_t*)ctx;
    if (a) a->attack_counter++;
    printf("FSM Transition -> STATE_ATTACK (count: %d)\n", a ? a->attack_counter : 0);
}

int main(void) {
    printf("=== Example 09: Particles, A* Pathfinding, and FSM Showcase ===\n");

    /* 1. Test Finite State Machine */
    mg_fsm_t* fsm = mg_fsm_create(STATE_IDLE);
    agent_context_t agent_ctx = { 0 };

    mg_fsm_state_desc_t s_idle = { .state_id = STATE_IDLE };
    mg_fsm_state_desc_t s_patrol = { .state_id = STATE_PATROL };
    mg_fsm_state_desc_t s_attack = { .state_id = STATE_ATTACK, .on_enter = on_attack_enter };

    mg_fsm_add_state(fsm, &s_idle);
    mg_fsm_add_state(fsm, &s_patrol);
    mg_fsm_add_state(fsm, &s_attack);

    mg_fsm_transition(fsm, STATE_PATROL, &agent_ctx);
    mg_fsm_transition(fsm, STATE_ATTACK, &agent_ctx);
    assert(mg_fsm_get_current_state(fsm) == STATE_ATTACK);
    assert(agent_ctx.attack_counter == 1);
    mg_fsm_destroy(fsm);
    printf("PASS: FSM state transitions verified\n");

    /* 2. Test A* Grid Pathfinding */
    uint8_t grid[10 * 10] = { 0 };
    /* Add an obstacle in column 5 */
    grid[2 * 10 + 5] = 1;
    grid[3 * 10 + 5] = 1;
    grid[4 * 10 + 5] = 1;

    mg_path_t path = mg_pathfind_astar(grid, 10, 10, 1, 3, 8, 3);
    printf("A* Pathfinding: %s, Total Steps: %u\n", path.path_found ? "Path Found" : "No Path", path.node_count);
    assert(path.path_found);
    assert(path.node_count >= 8);

    /* 3. Test Particle Emitter */
    mg_particle_emitter_t* emitter = mg_particle_emitter_create(128);
    mg_vec4_t fire_col = { 1.0f, 0.4f, 0.1f, 1.0f };
    mg_particle_emitter_burst(emitter, 100.0f, 100.0f, 64, 50.0f, &fire_col);

    assert(mg_particle_emitter_get_active_count(emitter) == 64);

    /* Simulate 30 frames of particle physics with gravity */
    for (int f = 0; f < 30; ++f) {
        mg_particle_emitter_tick(emitter, 0.016f, 9.8f);
    }
    printf("Particle Emitter: 64 particles simulated across 30 frames\n");
    mg_particle_emitter_destroy(emitter);

    printf("PASS: Example 09: Particles, A* Pathfinding, and FSM completed successfully\n");
    return 0;
}
