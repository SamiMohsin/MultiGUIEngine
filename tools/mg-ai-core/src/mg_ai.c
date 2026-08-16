/**
 * @file mg_ai.c
 * @brief MultiGUIEngine Local LLM Bridge implementation.
 */
#include "mg/ai/ai.h"
#include <stdlib.h>
#include <string.h>

#define MG_MAX_AI_TOOLS 32
#define MG_MAX_NAME_LEN 64

typedef struct {
    mg_ai_tool_desc_t desc;
    char name_buf[MG_MAX_NAME_LEN];
    bool is_active;
} mg_ai_registered_tool_t;

struct mg_ai_engine {
    char endpoint[128];
    char model[64];
    mg_ai_registered_tool_t tools[MG_MAX_AI_TOOLS];
    size_t tool_count;
    mg_subject_t* token_subject;
};

mg_ai_engine_t* mg_ai_create(const char* endpoint_url, const char* model_name) {
    mg_ai_engine_t* engine = (mg_ai_engine_t*)malloc(sizeof(mg_ai_engine_t));
    if (!engine) return NULL;

    memset(engine, 0, sizeof(mg_ai_engine_t));
    if (endpoint_url) strncpy(engine->endpoint, endpoint_url, sizeof(engine->endpoint) - 1);
    if (model_name) strncpy(engine->model, model_name, sizeof(engine->model) - 1);

    engine->token_subject = mg_subject_create();
    return engine;
}

void mg_ai_destroy(mg_ai_engine_t* engine) {
    if (!engine) return;

    if (engine->token_subject) {
        mg_subject_destroy(engine->token_subject);
    }
    free(engine);
}

bool mg_ai_register_tool(mg_ai_engine_t* engine, const mg_ai_tool_desc_t* desc) {
    if (!engine || !desc || !desc->name || !desc->fn) return false;

    for (size_t i = 0; i < MG_MAX_AI_TOOLS; ++i) {
        if (!engine->tools[i].is_active) {
            engine->tools[i].desc = *desc;
            strncpy(engine->tools[i].name_buf, desc->name, MG_MAX_NAME_LEN - 1);
            engine->tools[i].desc.name = engine->tools[i].name_buf;
            engine->tools[i].is_active = true;
            engine->tool_count++;
            return true;
        }
    }

    return false;
}

mg_subject_t* mg_ai_generate_stream(mg_ai_engine_t* engine, const char* prompt) {
    if (!engine || !prompt) return NULL;

    /* In headless/local simulation mode, tokenize prompt and emit */
    mg_ai_token_event_t ev1 = { .token = "AI: ", .is_done = false };
    mg_subject_emit(engine->token_subject, &ev1);

    mg_ai_token_event_t ev2 = { .token = prompt, .is_done = false };
    mg_subject_emit(engine->token_subject, &ev2);

    mg_ai_token_event_t ev_done = { .token = "\n", .is_done = true };
    mg_subject_emit(engine->token_subject, &ev_done);

    return engine->token_subject;
}

char* mg_ai_dispatch_tool(mg_ai_engine_t* engine, const char* tool_name, const char* arguments_json) {
    if (!engine || !tool_name) return NULL;

    for (size_t i = 0; i < MG_MAX_AI_TOOLS; ++i) {
        if (engine->tools[i].is_active && strcmp(engine->tools[i].desc.name, tool_name) == 0) {
            if (engine->tools[i].desc.fn) {
                return engine->tools[i].desc.fn(arguments_json, engine->tools[i].desc.ctx);
            }
        }
    }

    return NULL;
}
