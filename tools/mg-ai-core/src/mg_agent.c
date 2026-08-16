/**
 * @file mg_agent.c
 * @brief MultiGUIEngine Autonomous AI Agent, Multi-Turn Context Buffer & Tool Dispatch implementation.
 */
#include "mg/ai/agent.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct mg_ai_agent {
    mg_ai_chat_history_t history;
    mg_ai_agent_tool_def_t tools[MG_AI_MAX_TOOLS];
    uint32_t tool_count;
};

mg_ai_agent_t* mg_ai_agent_create(const char* system_prompt) {
    mg_ai_agent_t* agent = (mg_ai_agent_t*)calloc(1, sizeof(mg_ai_agent_t));
    if (agent) {
        if (system_prompt) {
            mg_ai_agent_add_message(agent, MG_AI_ROLE_SYSTEM, system_prompt);
        }
    }
    return agent;
}

void mg_ai_agent_destroy(mg_ai_agent_t* agent) {
    if (agent) free(agent);
}

bool mg_ai_agent_register_tool(
    mg_ai_agent_t* agent,
    const char* name,
    const char* description,
    mg_ai_agent_tool_cb_t callback,
    void* user_data
) {
    if (!agent || !name || !callback || agent->tool_count >= MG_AI_MAX_TOOLS) return false;
    mg_ai_agent_tool_def_t* t = &agent->tools[agent->tool_count++];
    strncpy(t->name, name, sizeof(t->name) - 1);
    if (description) {
        strncpy(t->description, description, sizeof(t->description) - 1);
    }
    t->callback = callback;
    t->user_data = user_data;
    return true;
}

void mg_ai_agent_add_message(mg_ai_agent_t* agent, mg_ai_role_t role, const char* content) {
    if (!agent || !content || agent->history.message_count >= MG_AI_MAX_MESSAGES) return;
    mg_ai_message_t* m = &agent->history.messages[agent->history.message_count++];
    m->role = role;
    strncpy(m->content, content, sizeof(m->content) - 1);
}

const char* mg_ai_agent_step(mg_ai_agent_t* agent, const char* user_input, char* out_buf, size_t out_buf_size) {
    if (!agent || !user_input || !out_buf || out_buf_size == 0) return NULL;

    mg_ai_agent_add_message(agent, MG_AI_ROLE_USER, user_input);

    /* Check if user prompt requests a registered tool */
    for (uint32_t i = 0; i < agent->tool_count; ++i) {
        if (strstr(user_input, agent->tools[i].name) != NULL) {
            char* tool_res = agent->tools[i].callback("{}", agent->tools[i].user_data);
            if (tool_res) {
                char tool_msg[512];
                snprintf(tool_msg, sizeof(tool_msg), "Tool [%s] Output: %s", agent->tools[i].name, tool_res);
                mg_ai_agent_add_message(agent, MG_AI_ROLE_TOOL, tool_msg);

                snprintf(out_buf, out_buf_size, "Executed %s: %s", agent->tools[i].name, tool_res);
                free(tool_res);
                mg_ai_agent_add_message(agent, MG_AI_ROLE_ASSISTANT, out_buf);
                return out_buf;
            }
        }
    }

    snprintf(out_buf, out_buf_size, "Acknowledged. Standing by for command.");
    mg_ai_agent_add_message(agent, MG_AI_ROLE_ASSISTANT, out_buf);
    return out_buf;
}
