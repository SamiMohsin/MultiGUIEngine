/**
 * @file main.c
 * @brief Example 24: Dynamic Plugin Lifecycle, Capability Negotiation & Event Broadcast.
 */
#include "mg/plugin/plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int g_telemetry_events_received = 0;
static float g_player_speed_multiplier = 1.0f;

/* Plugin 1: Gameplay Speed Booster Mod */
static bool speed_booster_init(mg_world_t* world) {
    (void)world;
    printf("[SpeedBoosterMod] Initialized! Applying 1.5x speed multiplier.\n");
    g_player_speed_multiplier = 1.5f;
    return true;
}

static void speed_booster_update(mg_world_t* world, float dt) {
    (void)world; (void)dt;
}

static void speed_booster_shutdown(mg_world_t* world) {
    (void)world;
    printf("[SpeedBoosterMod] Shutdown! Resetting speed multiplier to 1.0x.\n");
    g_player_speed_multiplier = 1.0f;
}

/* Plugin 2: Analytics & Telemetry Tracker */
static bool telemetry_init(mg_world_t* world) {
    (void)world;
    printf("[TelemetryTracker] Initialized! Listening for gameplay events.\n");
    return true;
}

static void telemetry_on_event(mg_world_t* world, const char* event_name, void* event_data) {
    (void)world;
    g_telemetry_events_received++;
    printf("[TelemetryTracker] Event Logged: '%s' (Payload: %s)\n", event_name, (const char*)event_data);
}

static void telemetry_shutdown(mg_world_t* world) {
    (void)world;
    printf("[TelemetryTracker] Shutdown! Total Events Recorded: %d\n", g_telemetry_events_received);
}

int main(void) {
    printf("=== Example 24: Dynamic Plugin Lifecycle & Event Broadcast ===\n");

    /* 1. Create ECS World and Plugin Manager */
    mg_world_t* world = mg_world_create();
    assert(world != NULL);

    mg_plugin_manager_t* mgr = mg_plugin_manager_create(world);
    assert(mgr != NULL);

    /* 2. Register Speed Booster Mod Plugin */
    mg_plugin_desc_t speed_mod = {
        .api_version = MG_PLUGIN_API_VERSION,
        .name = "SpeedBoosterMod",
        .version = "1.2.0",
        .author = "Community Modder",
        .init = speed_booster_init,
        .update = speed_booster_update,
        .shutdown = speed_booster_shutdown,
        .on_event = NULL
    };
    bool r1 = mg_plugin_register(mgr, &speed_mod);
    assert(r1 && g_player_speed_multiplier == 1.5f);

    /* 3. Register Telemetry Tracker Plugin */
    mg_plugin_desc_t telemetry_mod = {
        .api_version = MG_PLUGIN_API_VERSION,
        .name = "TelemetryTracker",
        .version = "2.0.1",
        .author = "Analytics Inc",
        .init = telemetry_init,
        .update = NULL,
        .shutdown = telemetry_shutdown,
        .on_event = telemetry_on_event
    };
    bool r2 = mg_plugin_register(mgr, &telemetry_mod);
    assert(r2);
    assert(mg_plugin_manager_count(mgr) == 2);
    printf("PASS: Registered 2 Dynamic Plugins (Active Count: %zu)\n", mg_plugin_manager_count(mgr));

    /* 4. Broadcast Custom Engine Events to Plugins */
    mg_plugin_manager_broadcast_event(mgr, "PLAYER_KILLED_BOSS", (void*)"{\"boss_id\": 99, \"loot\": \"plasma_cannon\"}");
    mg_plugin_manager_broadcast_event(mgr, "LEVEL_COMPLETED", (void*)"{\"level\": 5, \"time_sec\": 42.5}");
    assert(g_telemetry_events_received == 2);
    printf("PASS: Dispatched 2 Custom Engine Events across Plugin Host\n");

    /* 5. Update Cycle */
    mg_plugin_manager_update(mgr, 0.016f);

    /* 6. Dynamic Teardown */
    mg_plugin_manager_destroy(mgr);
    assert(g_player_speed_multiplier == 1.0f);
    printf("PASS: All Plugins Unloaded Cleanly (Speed multiplier restored: %.1f)\n", g_player_speed_multiplier);

    mg_world_destroy(world);

    printf("PASS: Example 24: Dynamic Plugin Lifecycle completed successfully\n");
    return 0;
}
