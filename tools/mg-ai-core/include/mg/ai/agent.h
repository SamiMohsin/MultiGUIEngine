/**
 * @file agent.h
 * @brief MultiGUIEngine Autonomous AI Agent, Multi-Turn Context Buffer & Tool Dispatch.
 */
#ifndef MG_AI_AGENT_H
#define MG_AI_AGENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_AI_MAX_MESSAGES 64
#define MG_AI_MAX_TOOLS 16

typedef enum mg_ai_role {
    MG_AI_ROLE_SYSTEM = 0,
    MG_AI_ROLE_USER,
    MG_AI_ROLE_ASSISTANT,
    MG_AI_ROLE_TOOL
} mg_ai_role_t;

typedef struct mg_ai_message {
    mg_ai_role_t role;
    char content[512];
    char tool_name[64];
} mg_ai_message_t;

typedef struct mg_ai_chat_history {
    mg_ai_message_t messages[MG_AI_MAX_MESSAGES];
    uint32_t message_count;
} mg_ai_chat_history_t;

typedef char* (*mg_ai_agent_tool_cb_t)(const char* args_json, void* user_data);

typedef struct mg_ai_agent_tool_def {
    char name[64];
    char description[256];
    mg_ai_agent_tool_cb_t callback;
    void* user_data;
} mg_ai_agent_tool_def_t;

typedef struct mg_ai_agent mg_ai_agent_t;

/**
 * @brief Creates an autonomous AI agent with system instructions.
 */
mg_ai_agent_t* mg_ai_agent_create(const char* system_prompt);

/**
 * @brief Destroys an AI agent and its context history.
 */
void mg_ai_agent_destroy(mg_ai_agent_t* agent);

/**
 * @brief Registers a native C tool callback with the agent.
 */
bool mg_ai_agent_register_tool(
    mg_ai_agent_t* agent,
    const char* name,
    const char* description,
    mg_ai_agent_tool_cb_t callback,
    void* user_data
);

/**
 * @brief Adds a message to the agent's multi-turn conversational history.
 */
void mg_ai_agent_add_message(mg_ai_agent_t* agent, mg_ai_role_t role, const char* content);

/**
 * @brief Processes user prompt, executes any matched tool calls, and produces assistant output.
 */
const char* mg_ai_agent_step(mg_ai_agent_t* agent, const char* user_input, char* out_buf, size_t out_buf_size);

#ifdef __cplusplus
}
#endif

#endif /* MG_AI_AGENT_H */
