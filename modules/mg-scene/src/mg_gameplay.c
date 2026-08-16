/**
 * @file mg_gameplay.c
 * @brief MultiGUIEngine Gameplay Subsystems implementation (FSM, A* Pathfinding, Particle Emitter).
 */
#include "mg/scene/gameplay.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_FSM_STATES 32

struct mg_fsm {
    uint32_t current_state;
    mg_fsm_state_desc_t states[MAX_FSM_STATES];
    uint32_t state_count;
};

mg_fsm_t* mg_fsm_create(uint32_t initial_state) {
    mg_fsm_t* fsm = (mg_fsm_t*)calloc(1, sizeof(mg_fsm_t));
    if (fsm) {
        fsm->current_state = initial_state;
    }
    return fsm;
}

void mg_fsm_destroy(mg_fsm_t* fsm) {
    if (fsm) free(fsm);
}

bool mg_fsm_add_state(mg_fsm_t* fsm, const mg_fsm_state_desc_t* desc) {
    if (!fsm || !desc || fsm->state_count >= MAX_FSM_STATES) return false;
    fsm->states[fsm->state_count++] = *desc;
    return true;
}

static const mg_fsm_state_desc_t* find_state(const mg_fsm_t* fsm, uint32_t state_id) {
    for (uint32_t i = 0; i < fsm->state_count; ++i) {
        if (fsm->states[i].state_id == state_id) return &fsm->states[i];
    }
    return NULL;
}

void mg_fsm_transition(mg_fsm_t* fsm, uint32_t new_state, void* ctx) {
    if (!fsm || fsm->current_state == new_state) return;
    const mg_fsm_state_desc_t* old_desc = find_state(fsm, fsm->current_state);
    if (old_desc && old_desc->on_exit) {
        old_desc->on_exit(ctx, 0.0f);
    }
    fsm->current_state = new_state;
    const mg_fsm_state_desc_t* new_desc = find_state(fsm, new_state);
    if (new_desc && new_desc->on_enter) {
        new_desc->on_enter(ctx, 0.0f);
    }
}

void mg_fsm_tick(mg_fsm_t* fsm, float dt, void* ctx) {
    if (!fsm) return;
    const mg_fsm_state_desc_t* s = find_state(fsm, fsm->current_state);
    if (s && s->on_update) {
        s->on_update(ctx, dt);
    }
}

uint32_t mg_fsm_get_current_state(const mg_fsm_t* fsm) {
    return fsm ? fsm->current_state : 0;
}

/* --- A* Grid Pathfinding Implementation --- */

typedef struct astar_node {
    int x, y;
    float g, h, f;
    int parent_x, parent_y;
    bool in_open;
    bool in_closed;
} astar_node_t;

mg_path_t mg_pathfind_astar(
    const uint8_t* grid,
    uint32_t grid_width,
    uint32_t grid_height,
    uint32_t start_x,
    uint32_t start_y,
    uint32_t target_x,
    uint32_t target_y
) {
    mg_path_t path;
    memset(&path, 0, sizeof(path));

    uint32_t total_cells = grid_width * grid_height;
    if (!grid || total_cells == 0 || total_cells > 4096 ||
        start_x >= grid_width || start_y >= grid_height ||
        target_x >= grid_width || target_y >= grid_height) {
        return path;
    }

    if (grid[start_y * grid_width + start_x] != 0 || grid[target_y * grid_width + target_x] != 0) {
        return path;
    }

    int parent[4096];
    bool visited[4096];
    int queue[4096];
    int head = 0, tail = 0;

    for (uint32_t i = 0; i < total_cells; ++i) {
        parent[i] = -1;
        visited[i] = false;
    }

    int start_idx = (int)(start_y * grid_width + start_x);
    int target_idx = (int)(target_y * grid_width + target_x);

    visited[start_idx] = true;
    queue[tail++] = start_idx;

    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    bool found = false;

    while (head < tail) {
        int curr = queue[head++];
        if (curr == target_idx) {
            found = true;
            break;
        }

        int cx = curr % (int)grid_width;
        int cy = curr / (int)grid_width;

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < (int)grid_width && ny >= 0 && ny < (int)grid_height) {
                int nidx = ny * (int)grid_width + nx;
                if (!visited[nidx] && grid[nidx] == 0) {
                    visited[nidx] = true;
                    parent[nidx] = curr;
                    queue[tail++] = nidx;
                }
            }
        }
    }

    if (found) {
        mg_grid_node_t temp_nodes[MG_MAX_PATH_NODES];
        uint32_t count = 0;
        int curr = target_idx;
        while (curr != -1 && count < MG_MAX_PATH_NODES) {
            temp_nodes[count++] = (mg_grid_node_t){ (uint32_t)(curr % (int)grid_width), (uint32_t)(curr / (int)grid_width) };
            curr = parent[curr];
        }

        /* Reverse into path */
        for (uint32_t i = 0; i < count; ++i) {
            path.nodes[i] = temp_nodes[count - 1 - i];
        }
        path.node_count = count;
        path.path_found = true;
    }

    return path;
}

/* --- Particle System Emitter Implementation --- */

struct mg_particle_emitter {
    mg_particle_t* particles;
    uint32_t max_particles;
};

mg_particle_emitter_t* mg_particle_emitter_create(uint32_t max_particles) {
    if (max_particles == 0) max_particles = 256;
    mg_particle_emitter_t* em = (mg_particle_emitter_t*)calloc(1, sizeof(mg_particle_emitter_t));
    if (em) {
        em->max_particles = max_particles;
        em->particles = (mg_particle_t*)calloc(max_particles, sizeof(mg_particle_t));
    }
    return em;
}

void mg_particle_emitter_destroy(mg_particle_emitter_t* emitter) {
    if (emitter) {
        if (emitter->particles) free(emitter->particles);
        free(emitter);
    }
}

void mg_particle_emitter_burst(
    mg_particle_emitter_t* emitter,
    float x, float y,
    uint32_t count,
    float speed,
    const mg_vec4_t* color
) {
    if (!emitter || !emitter->particles) return;
    uint32_t spawned = 0;

    for (uint32_t i = 0; i < emitter->max_particles && spawned < count; ++i) {
        if (!emitter->particles[i].active) {
            mg_particle_t* p = &emitter->particles[i];
            p->active = true;
            p->x = x;
            p->y = y;
            float angle = (float)spawned * (6.283185f / (float)count);
            p->vx = cosf(angle) * speed;
            p->vy = sinf(angle) * speed;
            p->life = 1.0f;
            p->max_life = 1.0f;
            p->color = color ? *color : (mg_vec4_t){ 1.0f, 0.8f, 0.2f, 1.0f };
            spawned++;
        }
    }
}

void mg_particle_emitter_tick(mg_particle_emitter_t* emitter, float dt, float gravity_y) {
    if (!emitter || !emitter->particles) return;
    for (uint32_t i = 0; i < emitter->max_particles; ++i) {
        mg_particle_t* p = &emitter->particles[i];
        if (p->active) {
            p->x += p->vx * dt;
            p->vy += gravity_y * dt;
            p->y += p->vy * dt;
            p->life -= dt;
            if (p->life <= 0.0f) {
                p->active = false;
            }
        }
    }
}

uint32_t mg_particle_emitter_get_active_count(const mg_particle_emitter_t* emitter) {
    if (!emitter || !emitter->particles) return 0;
    uint32_t active = 0;
    for (uint32_t i = 0; i < emitter->max_particles; ++i) {
        if (emitter->particles[i].active) active++;
    }
    return active;
}

const mg_particle_t* mg_particle_emitter_get_particles(const mg_particle_emitter_t* emitter) {
    return emitter ? emitter->particles : NULL;
}
