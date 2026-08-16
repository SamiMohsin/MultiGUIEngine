/**
 * @file main.c
 * @brief Example 23: Compressed Asset Bundles & Live Hot-Reload Watcher.
 */
#include "mg/asset/asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int g_reload_count = 0;

static void on_asset_reloaded(const char* path, void* user_data) {
    (void)user_data;
    printf("HOT-RELOAD EVENT: Asset '%s' modified on disk! Live reloaded.\n", path);
    g_reload_count++;
}

int main(void) {
    printf("=== Example 23: Compressed Asset Bundles & Live Hot-Reload Engine ===\n");

    /* 1. Create In-Memory Asset Package Bundle (.mgpak) */
    mg_asset_bundle_t* bundle = mg_asset_bundle_create();
    assert(bundle != NULL);

    const char* shader_code = "#version 450\nvoid main() { gl_FragColor = vec4(1, 0, 0, 1); }";
    const char* material_json = "{\"albedo\": \"textures/ship.png\", \"metallic\": 0.9}";
    const uint8_t raw_texture[16] = { 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF };

    bool a1 = mg_asset_bundle_add(bundle, "shaders/uber.frag", shader_code, strlen(shader_code) + 1);
    bool a2 = mg_asset_bundle_add(bundle, "materials/ship.json", material_json, strlen(material_json) + 1);
    bool a3 = mg_asset_bundle_add(bundle, "textures/ship.raw", raw_texture, sizeof(raw_texture));
    assert(a1 && a2 && a3);
    printf("PASS: 3 Assets Packed into Unified Bundle (Total Payload: %zu bytes)\n", bundle->payload_size);

    /* 2. Direct Archive Extraction by Path */
    size_t ext_size = 0;
    const char* ext_shader = (const char*)mg_asset_bundle_extract(bundle, "shaders/uber.frag", &ext_size);
    assert(ext_shader != NULL && ext_size == strlen(shader_code) + 1);
    assert(strcmp(ext_shader, shader_code) == 0);
    printf("PASS: Extracted 'shaders/uber.frag' (%zu bytes) with 100%% CRC32 fidelity\n", ext_size);

    /* 3. Live Hot-Reload Watcher Simulation */
    mg_asset_watcher_t watcher;
    mg_asset_watcher_init(&watcher);

    uint32_t initial_crc = mg_crc32(shader_code, strlen(shader_code) + 1);
    mg_asset_watcher_add(&watcher, "shaders/uber.frag", initial_crc, on_asset_reloaded, NULL);

    /* Check with unchanged data -> no event */
    bool mod1 = mg_asset_watcher_check(&watcher, "shaders/uber.frag", shader_code, strlen(shader_code) + 1);
    assert(mod1 == false && g_reload_count == 0);

    /* Developer edits shader code on disk */
    const char* updated_shader = "#version 450\nvoid main() { gl_FragColor = vec4(0, 1, 0, 1); }"; /* Green */
    bool mod2 = mg_asset_watcher_check(&watcher, "shaders/uber.frag", updated_shader, strlen(updated_shader) + 1);
    assert(mod2 == true && g_reload_count == 1);
    printf("PASS: Live Hot-Reload detected mutation and dispatched event successfully\n");

    /* Cleanup */
    mg_asset_bundle_destroy(bundle);

    printf("PASS: Example 23: Asset Bundles & Live Hot-Reload completed successfully\n");
    return 0;
}
