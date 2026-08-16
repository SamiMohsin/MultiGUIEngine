/**
 * @file ai.h
 * @brief MultiGUIEngine Local LLM Bridge, Function Calling, and Streaming Subsystem.
 */
#ifndef MG_AI_AI_H
#define MG_AI_AI_H

#include "mg/rx/rx.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_ai_engine mg_ai_engine_t;

typedef struct mg_ai_token_event {
    const char* token;
    bool is_done;
} mg_ai_token_event_t;

typedef struct mg_ai_tool_call {
    const char* tool_name;
    const char* arguments_json;
} mg_ai_tool_call_t;

typedef char* (*mg_ai_tool_fn)(const char* arguments_json, void* ctx);

typedef struct mg_ai_tool_desc {
    const char* name;
    const char* description;
    const char* parameters_schema_json;
    mg_ai_tool_fn fn;
    void* ctx;
} mg_ai_tool_desc_t;

/**
 * @brief Creates an AI client engine.
 * @param endpoint_url LLM server endpoint (e.g. "http://localhost:11434").
 * @param model_name Model identifier (e.g. "qwen2.5-coder:7b").
 */
mg_ai_engine_t* mg_ai_create(const char* endpoint_url, const char* model_name);

/**
 * @brief Destroys an AI client engine.
 */
void mg_ai_destroy(mg_ai_engine_t* engine);

/**
 * @brief Registers a native C tool callback for LLM function calling.
 */
bool mg_ai_register_tool(mg_ai_engine_t* engine, const mg_ai_tool_desc_t* desc);

/**
 * @brief Executes a prompt and streams tokens to a reactive subject.
 * @param engine Pointer to engine.
 * @param prompt User prompt text.
 * @return Observable stream of mg_ai_token_event_t.
 */
mg_subject_t* mg_ai_generate_stream(mg_ai_engine_t* engine, const char* prompt);

/**
 * @brief Dispatches a tool call to the registered C function.
 */
char* mg_ai_dispatch_tool(mg_ai_engine_t* engine, const char* tool_name, const char* arguments_json);

#ifdef __cplusplus
}
#endif

#endif /* MG_AI_AI_H */
