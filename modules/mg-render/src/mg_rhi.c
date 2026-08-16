/**
 * @file mg_rhi.c
 * @brief Global RHI API dispatch implementation.
 */
#include "mg/render/rhi.h"
#include "mg/render/backend_software.h"
#include <string.h>

static const mg_rhi_backend_t* g_active_backend = NULL;

bool mg_rhi_init(const mg_rhi_desc_t* desc) {
    if (!desc) return false;

    if (desc->backend_type == MG_RHI_BACKEND_SOFTWARE) {
        g_active_backend = mg_backend_software_get();
    } else {
        /* Other backends fall back to software if not yet initialized */
        g_active_backend = mg_backend_software_get();
    }

    if (g_active_backend && g_active_backend->init) {
        return g_active_backend->init(desc);
    }
    return false;
}

void mg_rhi_shutdown(void) {
    if (g_active_backend && g_active_backend->shutdown) {
        g_active_backend->shutdown();
    }
    g_active_backend = NULL;
}

mg_buffer_handle_t mg_rhi_create_buffer(const mg_buffer_desc_t* desc) {
    if (g_active_backend && g_active_backend->create_buffer) {
        return g_active_backend->create_buffer(desc);
    }
    return MG_INVALID_HANDLE;
}

bool mg_rhi_update_buffer(mg_buffer_handle_t handle, size_t offset, const void* data, size_t size) {
    if (g_active_backend && g_active_backend->update_buffer) {
        return g_active_backend->update_buffer(handle, offset, data, size);
    }
    return false;
}

void mg_rhi_destroy_buffer(mg_buffer_handle_t handle) {
    if (g_active_backend && g_active_backend->destroy_buffer) {
        g_active_backend->destroy_buffer(handle);
    }
}

mg_texture_handle_t mg_rhi_create_texture(const mg_texture_desc_t* desc) {
    if (g_active_backend && g_active_backend->create_texture) {
        return g_active_backend->create_texture(desc);
    }
    return MG_INVALID_HANDLE;
}

void mg_rhi_destroy_texture(mg_texture_handle_t handle) {
    if (g_active_backend && g_active_backend->destroy_texture) {
        g_active_backend->destroy_texture(handle);
    }
}

mg_pipeline_handle_t mg_rhi_create_pipeline(const mg_pipeline_desc_t* desc) {
    if (g_active_backend && g_active_backend->create_pipeline) {
        return g_active_backend->create_pipeline(desc);
    }
    return MG_INVALID_HANDLE;
}

void mg_rhi_destroy_pipeline(mg_pipeline_handle_t handle) {
    if (g_active_backend && g_active_backend->destroy_pipeline) {
        g_active_backend->destroy_pipeline(handle);
    }
}

void mg_rhi_begin_frame(const mg_color_t* clear_color) {
    if (g_active_backend && g_active_backend->begin_frame) {
        g_active_backend->begin_frame(clear_color);
    }
}

void mg_rhi_draw(const mg_draw_cmd_t* cmd) {
    if (g_active_backend && g_active_backend->draw) {
        g_active_backend->draw(cmd);
    }
}

void mg_rhi_end_frame(void) {
    if (g_active_backend && g_active_backend->end_frame) {
        g_active_backend->end_frame();
    }
}

bool mg_rhi_read_pixels(void* out_buffer, size_t buffer_size) {
    if (g_active_backend && g_active_backend->read_pixels) {
        return g_active_backend->read_pixels(out_buffer, buffer_size);
    }
    return false;
}
