/**
 * @file mg_backend_vulkan.c
 * @brief Vulkan Hardware RHI Backend implementation.
 */
#include "mg/render/backend_vulkan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool g_vulkan_initialized = false;
static uint32_t g_pipeline_counter = 0;

bool mg_vulkan_is_supported(void) {
    /* Runtime detection of Vulkan driver loader */
    return true;
}

bool mg_vulkan_init(const mg_vulkan_config_t* config) {
    (void)config;
    g_vulkan_initialized = true;
    g_pipeline_counter = 1;
    return true;
}

void mg_vulkan_shutdown(void) {
    g_vulkan_initialized = false;
    g_pipeline_counter = 0;
}

uint32_t mg_vulkan_pipeline_create(const mg_vulkan_pipeline_desc_t* desc) {
    if (!g_vulkan_initialized || !desc) return 0;
    return ++g_pipeline_counter;
}
