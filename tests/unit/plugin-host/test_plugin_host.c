/**
 * @file test_plugin_host.c
 * @brief Comprehensive headless unit tests for plugin-host.
 */
#include "mg/plugin/plugin.h"
#include <stdio.h>
#include <stdlib.h>

static int g_plugin_init_called = 0;
static int g_plugin_update_called = 0;
static int g_plugin_shutdown_called = 0;

static bool sample_plugin_init(mg_world_t* world) {
    (void)world;
    g_plugin_init_called++;
    return true;
}

static void sample_plugin_update(mg_world_t* world, float dt) {
    (void)world; (void)dt;
    g_plugin_update_called++;
}

static void sample_plugin_shutdown(mg_world_t* world) {
    (void)world;
    g_plugin_shutdown_called++;
}

static int test_plugin_lifecycle(void) {
    printf("Testing plugin lifecycle callbacks...\n");

    mg_world_t* world = mg_world_create();
    if (!world) return 1;

    mg_plugin_manager_t* mgr = mg_plugin_manager_create(world);
    if (!mgr) {
        mg_world_destroy(world);
        return 1;
    }

    mg_plugin_desc_t desc = {
        .api_version = MG_PLUGIN_API_VERSION,
        .name = "SamplePlugin",
        .version = "1.0.0",
        .author = "MultiGUIEngine Team",
        .init = sample_plugin_init,
        .update = sample_plugin_update,
        .shutdown = sample_plugin_shutdown
    };

    if (!mg_plugin_register(mgr, &desc)) {
        fprintf(stderr, "FAIL: mg_plugin_register failed\n");
        mg_plugin_manager_destroy(mgr);
        mg_world_destroy(world);
        return 1;
    }

    if (g_plugin_init_called != 1) {
        fprintf(stderr, "FAIL: init was not called\n");
        return 1;
    }

    mg_plugin_manager_update(mgr, 0.016f);
    if (g_plugin_update_called != 1) {
        fprintf(stderr, "FAIL: update was not called\n");
        return 1;
    }

    mg_plugin_manager_destroy(mgr);
    mg_world_destroy(world);

    if (g_plugin_shutdown_called != 1) {
        fprintf(stderr, "FAIL: shutdown was not called\n");
        return 1;
    }

    printf("PASS: plugin lifecycle callbacks\n");
    return 0;
}

int main(void) {
    if (test_plugin_lifecycle() != 0) return 1;

    printf("All plugin-host unit tests passed successfully!\n");
    return 0;
}
