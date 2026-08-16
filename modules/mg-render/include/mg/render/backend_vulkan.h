/**
 * @file backend_vulkan.h
 * @brief Vulkan Modern Hardware RHI Backend interface for MultiGUIEngine.
 */
#ifndef MG_RENDER_BACKEND_VULKAN_H
#define MG_RENDER_BACKEND_VULKAN_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_vulkan_config {
    const char* app_name;
    uint32_t app_version;
    bool enable_validation_layers;
    bool prefer_discrete_gpu;
} mg_vulkan_config_t;

typedef struct mg_vulkan_pipeline_desc {
    const void* vert_spirv_data;
    size_t vert_spirv_size;
    const void* frag_spirv_data;
    size_t frag_spirv_size;
    bool depth_test_enable;
    bool blend_enable;
} mg_vulkan_pipeline_desc_t;

/**
 * @brief Initializes Vulkan RHI device context. Returns false if hardware driver is not found.
 */
bool mg_vulkan_init(const mg_vulkan_config_t* config);

/**
 * @brief Shuts down Vulkan RHI device context.
 */
void mg_vulkan_shutdown(void);

/**
 * @brief Creates a Vulkan graphics pipeline from SPIR-V bytecodes.
 */
uint32_t mg_vulkan_pipeline_create(const mg_vulkan_pipeline_desc_t* desc);

/**
 * @brief Checks if Vulkan hardware support is available on current system.
 */
bool mg_vulkan_is_supported(void);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_BACKEND_VULKAN_H */
