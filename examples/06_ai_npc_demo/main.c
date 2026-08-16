/**
 * @file main.c
 * @brief Example 06: AI NPC Dialogue and Tool Calling Demonstration.
 */
#include "mg/ai/ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_inventory_potions = 0;

static char* give_potion_tool(const char* args_json, void* ctx) {
    (void)args_json; (void)ctx;
    g_inventory_potions += 3;
    return strdup("{\"success\": true, \"message\": \"Gave 3 Health Potions to player\"}");
}

static void on_npc_dialogue_token(void* val, void* ctx) {
    (void)ctx;
    const mg_ai_token_event_t* ev = (const mg_ai_token_event_t*)val;
    if (ev && ev->token) {
        printf("%s", ev->token);
    }
}

int main(void) {
    printf("Running Example 06: AI NPC Demo...\n");

    mg_ai_engine_t* ai = mg_ai_create("http://localhost:11434", "qwen2.5:7b");
    if (!ai) {
        fprintf(stderr, "FAIL: mg_ai_create failed\n");
        return 1;
    }

    /* Register NPC tool */
    mg_ai_tool_desc_t potion_tool = {
        .name = "give_health_potions",
        .description = "Gives health potions to player in need",
        .parameters_schema_json = "{}",
        .fn = give_potion_tool,
        .ctx = NULL
    };
    mg_ai_register_tool(ai, &potion_tool);

    /* Simulate NPC deciding to reward player */
    char* tool_res = mg_ai_dispatch_tool(ai, "give_health_potions", "{}");
    if (!tool_res || g_inventory_potions != 3) {
        fprintf(stderr, "FAIL: give_health_potions tool failed (inventory: %d)\n", g_inventory_potions);
        if (tool_res) free(tool_res);
        mg_ai_destroy(ai);
        return 1;
    }
    free(tool_res);

    /* Stream NPC dialogue */
    mg_subject_t* stream = mg_ai_generate_stream(ai, "Take these potions, traveler! Stay safe in the dungeons.");
    mg_subscription_t* sub = mg_subject_subscribe(stream, on_npc_dialogue_token, NULL);

    /* Trigger dialogue emission */
    mg_ai_generate_stream(ai, "Farewell!");

    mg_subscription_dispose(sub);
    mg_ai_destroy(ai);

    printf("PASS: Example 06: AI NPC Dialogue and Tool Calling completed (Potions rewarded: %d)\n", g_inventory_potions);
    return 0;
}
