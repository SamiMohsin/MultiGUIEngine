/**
 * @file main.c
 * @brief Example 17: Autonomous AI Function Calling Agent & Multi-Turn Memory.
 */
#include "mg/ai/ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Simulated game state */
typedef struct game_state {
    int shield_level;
    int warp_fuel;
} game_state_t;

static char* tool_query_shields(const char* args_json, void* user_data) {
    (void)args_json;
    game_state_t* state = (game_state_t*)user_data;
    char* res = (char*)malloc(128);
    snprintf(res, 128, "{\"shield_percent\": %d, \"status\": \"OPTIMAL\"}", state->shield_level);
    return res;
}

static char* tool_recharge_shields(const char* args_json, void* user_data) {
    (void)args_json;
    game_state_t* state = (game_state_t*)user_data;
    state->shield_level = 100;
    char* res = (char*)malloc(128);
    snprintf(res, 128, "{\"action\": \"recharged\", \"new_shield_percent\": 100}");
    return res;
}

int main(void) {
    printf("=== Example 17: Autonomous AI Function Calling Agent & Multi-Turn Memory ===\n");

    game_state_t state = { .shield_level = 42, .warp_fuel = 85 };

    /* 1. Initialize Autonomous AI Agent */
    mg_ai_agent_t* agent = mg_ai_agent_create("You are AstroCopilot, an onboard AI tactical assistant for starship AstroGuard.");
    assert(agent != NULL);

    /* 2. Register Native C Tool Callbacks */
    bool r1 = mg_ai_agent_register_tool(agent, "query_shields", "Queries starship shield level and integrity status", tool_query_shields, &state);
    bool r2 = mg_ai_agent_register_tool(agent, "recharge_shields", "Restores starship shields to 100% capacity", tool_recharge_shields, &state);
    assert(r1 && r2);
    printf("PASS: Native C Tool Callbacks registered (query_shields, recharge_shields)\n");

    /* 3. Turn 1: Query Shields */
    char response_buf[512];
    const char* out1 = mg_ai_agent_step(agent, "Copilot, query_shields status immediately", response_buf, sizeof(response_buf));
    assert(out1 != NULL);
    assert(strstr(out1, "42") != NULL);
    printf("Agent Response 1: %s\n", out1);

    /* 4. Turn 2: Recharge Shields */
    const char* out2 = mg_ai_agent_step(agent, "Shields critical, execute recharge_shields", response_buf, sizeof(response_buf));
    assert(out2 != NULL);
    assert(state.shield_level == 100);
    printf("Agent Response 2: %s\n", out2);

    /* 5. Turn 3: Conversational query */
    const char* out3 = mg_ai_agent_step(agent, "Great work, standing by", response_buf, sizeof(response_buf));
    assert(out3 != NULL);
    printf("Agent Response 3: %s\n", out3);

    /* Cleanup */
    mg_ai_agent_destroy(agent);

    printf("PASS: Example 17: Autonomous AI Function Calling Agent completed successfully\n");
    return 0;
}
