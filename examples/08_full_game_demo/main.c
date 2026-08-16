/**
 * @file main.c
 * @brief Example 08: AstroGuard — Full Game Showcase integrating all MultiGUIEngine subsystems.
 */
#include "mg/render/render.h"
#include "mg/scene/scene.h"
#include "mg/physics/physics.h"
#include "mg/audio/audio.h"
#include "mg/input/input.h"
#include "mg/ui/ui.h"
#include "mg/ai/ai.h"
#include "mg/pal/pal.h"
#include "mg/alloc/alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define COMP_POS 0
#define COMP_VEL 1
#define COMP_STAT 2

typedef struct {
    float x, y;
} pos_comp_t;

typedef struct {
    float vx, vy;
} vel_comp_t;

typedef struct {
    float health;
    int score;
    bool is_player;
    bool is_asteroid;
} stat_comp_t;

/* System: Movement and boundary bounce */
static void movement_system(mg_world_t* world, float dt, void* ctx) {
    (void)ctx;
    for (uint32_t i = 1; i < 64; ++i) {
        if (!mg_entity_is_alive(world, i)) continue;
        pos_comp_t* p = (pos_comp_t*)mg_component_get(world, i, COMP_POS);
        vel_comp_t* v = (vel_comp_t*)mg_component_get(world, i, COMP_VEL);
        if (p && v) {
            p->x += v->vx * dt;
            p->y += v->vy * dt;

            /* Screen boundary bounce for asteroids */
            stat_comp_t* s = (stat_comp_t*)mg_component_get(world, i, COMP_STAT);
            if (s && s->is_asteroid) {
                if (p->x < 50.0f || p->x > 1230.0f) v->vx = -v->vx;
                if (p->y < 100.0f || p->y > 620.0f) v->vy = -v->vy;
            }
        }
    }
}

/* AI Copilot tool for emergency shield replenishment */
static char* emergency_shield_boost(const char* args_json, void* user_data) {
    (void)args_json;
    stat_comp_t* player_stat = (stat_comp_t*)user_data;
    if (player_stat) {
        player_stat->health = 100.0f;
    }
    return strdup("{\"status\":\"success\",\"shield_level\":100}");
}

int main(void) {
    printf("=== Example 08: AstroGuard Full Playable Game Showcase ===\n");

    const uint32_t screen_w = 1280;
    const uint32_t screen_h = 720;

    /* 1. Core PAL & CPU caps */
    mg_cpu_caps_t cpu = mg_cpu_get_capabilities();
    printf("Initializing on %s (%u logical cores)...\n", cpu.arch_name, cpu.logical_cores);

    /* 2. RHI Software Render Backend */
    mg_rhi_desc_t rhi_desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = screen_w,
        .height = screen_h,
        .vsync = false,
        .offscreen = true
    };
    if (!mg_rhi_init(&rhi_desc)) {
        fprintf(stderr, "FAIL: mg_rhi_init failed\n");
        return 1;
    }

    mg_batch2d_t* batch = mg_batch2d_create(2048);
    mg_ui_context_t* ui = mg_ui_create(NULL);
    mg_audio_desc_t audio_desc = { .sample_rate = 44100, .channels = 2 };
    mg_audio_engine_t* audio = mg_audio_create(&audio_desc);

    /* 2.5 Audio Sound Setup */
    float pcm[256];
    for (int s = 0; s < 256; ++s) {
        pcm[s] = sinf((float)s * 0.2f) * 0.4f;
    }
    mg_sound_desc_t sdesc = {
        .pcm_samples = pcm,
        .sample_count = 256,
        .sample_rate = 44100,
        .channels = 1
    };
    mg_sound_handle_t laser_snd = mg_audio_sound_create(audio, &sdesc);

    /* 3. ECS World Setup */
    mg_world_t* world = mg_world_create();
    mg_component_register(world, COMP_POS, sizeof(pos_comp_t));
    mg_component_register(world, COMP_VEL, sizeof(vel_comp_t));
    mg_component_register(world, COMP_STAT, sizeof(stat_comp_t));
    mg_system_register(world, movement_system, NULL, MG_STAGE_UPDATE);

    /* 4. Spawn Player Entity */
    mg_entity_t player = mg_entity_create(world);
    pos_comp_t* p_pos = (pos_comp_t*)mg_component_add(world, player, COMP_POS);
    p_pos->x = 640.0f; p_pos->y = 360.0f;
    vel_comp_t* p_vel = (vel_comp_t*)mg_component_add(world, player, COMP_VEL);
    p_vel->vx = 20.0f; p_vel->vy = 10.0f;
    stat_comp_t* p_stat = (stat_comp_t*)mg_component_add(world, player, COMP_STAT);
    p_stat->health = 85.0f;
    p_stat->score = 1450;
    p_stat->is_player = true;

    /* 5. Spawn Asteroids */
    for (int i = 0; i < 6; ++i) {
        mg_entity_t ast = mg_entity_create(world);
        pos_comp_t* apos = (pos_comp_t*)mg_component_add(world, ast, COMP_POS);
        apos->x = 150.0f + (float)i * 180.0f;
        apos->y = 150.0f + (float)(i % 3) * 120.0f;
        vel_comp_t* avel = (vel_comp_t*)mg_component_add(world, ast, COMP_VEL);
        avel->vx = (float)((i % 2 == 0) ? 60 : -45);
        avel->vy = (float)((i % 3 == 0) ? 50 : -35);
        stat_comp_t* astat = (stat_comp_t*)mg_component_add(world, ast, COMP_STAT);
        astat->is_asteroid = true;
    }

    /* 6. AI Copilot Integration */
    mg_ai_engine_t* ai = mg_ai_create("http://localhost:11434", "qwen2.5-coder:7b");
    if (ai) {
        mg_ai_tool_desc_t tool = {
            .name = "emergency_shield_boost",
            .description = "Restores spaceship shields to 100%",
            .parameters_schema_json = "{}",
            .fn = emergency_shield_boost,
            .ctx = p_stat
        };
        mg_ai_register_tool(ai, &tool);
        char* tool_res = mg_ai_dispatch_tool(ai, "emergency_shield_boost", "{}");
        if (tool_res) free(tool_res);
    }

    /* 7. Simulation Loop: 120 Frames */
    for (int frame = 0; frame < 120; ++frame) {
        /* Run ECS world update */
        mg_world_tick(world, 0.016f);

        /* Laser audio pulse */
        if (frame % 30 == 0 && audio && laser_snd) {
            mg_audio_play(audio, laser_snd, MG_AUDIO_BUS_SFX, 0.8f, 1.0f, 0.0f, false);
        }
    }

    /* 8. Render Game & Material 3 HUD Frame */
    mg_color_t space_black = { 0.02f, 0.02f, 0.04f, 1.0f };
    mg_rhi_begin_frame(&space_black);
    mg_batch2d_begin(batch);

    /* Draw Starfield Background Quads */
    for (int s = 0; s < 30; ++s) {
        float sx = (float)((s * 97) % 1240) + 20.0f;
        float sy = (float)((s * 67) % 680) + 20.0f;
        mg_color_t star_col = { 0.7f, 0.7f, 0.9f, 0.6f };
        mg_batch2d_draw_quad(batch, sx, sy, 3.0f, 3.0f, &star_col);
    }

    /* Draw Asteroids */
    for (uint32_t i = 1; i < 64; ++i) {
        if (!mg_entity_is_alive(world, i)) continue;
        stat_comp_t* s = (stat_comp_t*)mg_component_get(world, i, COMP_STAT);
        pos_comp_t* p = (pos_comp_t*)mg_component_get(world, i, COMP_POS);
        if (s && s->is_asteroid && p) {
            mg_color_t ast_col = { 0.6f, 0.45f, 0.35f, 1.0f };
            mg_batch2d_draw_quad(batch, p->x - 16.0f, p->y - 16.0f, 32.0f, 32.0f, &ast_col);
        }
    }

    /* Draw Player Ship */
    mg_color_t ship_col = { 0.2f, 0.8f, 1.0f, 1.0f };
    mg_batch2d_draw_quad(batch, p_pos->x - 20.0f, p_pos->y - 20.0f, 40.0f, 40.0f, &ship_col);

    /* Draw Material 3 HUD Overlay */
    mg_ui_begin(ui, (float)screen_w, (float)screen_h);

    /* Top HUD Bar */
    mg_ui_m3_top_app_bar(ui, "ASTROGUARD: SECTOR 7", MG_ICON_MENU, MG_ICON_SETTINGS, (float)screen_w);

    /* Health Progress & Shields */
    mg_ui_m3_progress_linear(ui, p_stat->health / 100.0f, 30.0f, 75.0f, 220.0f, 8.0f);
    bool shield_active = true;
    mg_ui_m3_switch(ui, "Shields Active", &shield_active, 270.0f, 68.0f);

    /* Score Badge */
    mg_ui_m3_badge(ui, p_stat->score / 100, 480.0f, 72.0f);
    mg_ui_m3_chip(ui, "Hyperdrive Ready", MG_ICON_CHECK, MG_M3_CHIP_FILTER, true, 520.0f, 68.0f);

    /* Mini Radar Card */
    mg_ui_m3_card(ui, MG_M3_CARD_ELEVATED, 1020.0f, 80.0f, 230.0f, 160.0f);
    mg_ui_m3_button(ui, "Sector Radar", MG_ICON_SEARCH, MG_M3_BTN_TONAL, 1030.0f, 90.0f, 210.0f, 32.0f);

    /* AI Transmission Snackbar */
    mg_ui_m3_snackbar(ui, "[AI Copilot] Emergency shields restored to 100%. Sector clear.", "Acknowledge", (float)screen_w, (float)screen_h);

    mg_ui_render(ui, batch);
    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    /* Readback pixel to verify render */
    uint32_t* pixels = (uint32_t*)malloc(screen_w * screen_h * sizeof(uint32_t));
    if (pixels) {
        if (mg_rhi_read_pixels(pixels, screen_w * screen_h * sizeof(uint32_t))) {
            uint32_t hud_px = pixels[20 * screen_w + 100];
            uint32_t space_px = pixels[500 * screen_w + 500];
            printf("Game Frame Rendered: HUD Pixel=0x%08X, Space Pixel=0x%08X\n", hud_px, space_px);
        }
        free(pixels);
    }

    /* Cleanup */
    if (ai) mg_ai_destroy(ai);
    mg_world_destroy(world);
    if (audio) mg_audio_destroy(audio);
    mg_ui_destroy(ui);
    mg_batch2d_destroy(batch);
    mg_rhi_shutdown();

    printf("PASS: Example 08: AstroGuard Full Game Showcase completed successfully (Score: %d, Health: %.0f)\n",
           p_stat->score, p_stat->health);
    return 0;
}
