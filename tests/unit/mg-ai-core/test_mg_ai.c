/**
 * @file test_mg_ai.c
 * @brief Comprehensive headless unit tests for mg-ai-core.
 */
#include "mg/ai/ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* sample_get_weather(const char* args_json, void* ctx) {
    (void)args_json; (void)ctx;
    return strdup("{\"temperature\": 22.5, \"condition\": \"Sunny\"}");
}

static int g_ai_token_count = 0;
static void on_ai_token(void* val, void* ctx) {
    (void)ctx;
    const mg_ai_token_event_t* ev = (const mg_ai_token_event_t*)val;
    if (ev) {
        g_ai_token_count++;
    }
}

static int test_ai_tool_and_streaming(void) {
    printf("Testing AI tool registration, dispatch, and token streaming...\n");

    mg_ai_engine_t* ai = mg_ai_create("http://localhost:11434", "qwen2.5:7b");
    if (!ai) return 1;

    mg_ai_tool_desc_t tool_desc = {
        .name = "get_weather",
        .description = "Returns current weather data",
        .parameters_schema_json = "{}",
        .fn = sample_get_weather,
        .ctx = NULL
    };

    if (!mg_ai_register_tool(ai, &tool_desc)) {
        fprintf(stderr, "FAIL: mg_ai_register_tool failed\n");
        mg_ai_destroy(ai);
        return 1;
    }

    char* result = mg_ai_dispatch_tool(ai, "get_weather", "{\"location\": \"Tokyo\"}");
    if (!result || strstr(result, "Sunny") == NULL) {
        fprintf(stderr, "FAIL: tool dispatch returned invalid result\n");
        if (result) free(result);
        mg_ai_destroy(ai);
        return 1;
    }
    free(result);

    /* Stream prompt */
    mg_subject_t* stream = mg_ai_generate_stream(ai, "Hello world");
    mg_subscription_t* sub = mg_subject_subscribe(stream, on_ai_token, NULL);

    /* Trigger another stream to verify subscriber notification */
    mg_ai_generate_stream(ai, "How are you?");
    if (g_ai_token_count < 3) {
        fprintf(stderr, "FAIL: token stream received %d tokens < 3\n", g_ai_token_count);
        mg_subscription_dispose(sub);
        mg_ai_destroy(ai);
        return 1;
    }

    mg_subscription_dispose(sub);
    mg_ai_destroy(ai);

    printf("PASS: AI tool registration, dispatch, and token streaming\n");
    return 0;
}

int main(void) {
    if (test_ai_tool_and_streaming() != 0) return 1;

    printf("All mg-ai-core unit tests passed successfully!\n");
    return 0;
}
