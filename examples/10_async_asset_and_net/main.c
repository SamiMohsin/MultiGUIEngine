/**
 * @file main.c
 * @brief Example 10: Async Asset Streaming, Network ECS Replication, and Post-Processing.
 */
#include "mg/asset/asset.h"
#include "mg/net/net.h"
#include "mg/render/render.h"
#include "mg/pal/pal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void) {
    printf("=== Example 10: Async Asset Streaming, Network Replication, and Post-Processing ===\n");

    /* 1. Test Asynchronous Asset Loading */
    const char* sample_asset_file = "/tmp/sample_async_asset.txt";
    mg_fs_write_file(sample_asset_file, "ASYNC_STREAM_DATA_OK", 20);

    mg_asset_manager_t* asset_mgr = mg_asset_manager_create("/tmp");
    mg_asset_handle_t h_asset = mg_asset_load_async(asset_mgr, "sample_async_asset.txt", MG_ASSET_TYPE_TEXT);
    assert(h_asset != MG_INVALID_ASSET);

    const mg_asset_info_t* ainfo = mg_asset_get(asset_mgr, h_asset);
    assert(ainfo != NULL);
    assert(ainfo->state == MG_ASSET_STATE_LOADED);
    printf("PASS: Async Asset Streaming verified (Path: %s, Size: %zu bytes)\n", ainfo->path, ainfo->size);
    mg_asset_manager_destroy(asset_mgr);

    /* 2. Test ECS Network Replication (Server -> Client) */
    mg_net_replicator_t* server_rep = mg_net_replicator_create(true);
    mg_net_replicator_t* client_rep = mg_net_replicator_create(false);

    mg_net_entity_snapshot_t srv_snapshots[2] = {
        { .net_id = 101, .x = 12.5f, .y = 34.0f, .z = 0.0f, .vx = 2.0f, .vy = 0.0f, .vz = 0.0f, .health = 100, .state_flags = 1 },
        { .net_id = 102, .x = 88.0f, .y = 90.0f, .z = 0.0f, .vx = -1.0f, .vy = 0.0f, .vz = 0.0f, .health = 75, .state_flags = 2 }
    };

    uint8_t packet_buffer[512];
    size_t written = mg_net_replicate_serialize(server_rep, srv_snapshots, 2, packet_buffer, sizeof(packet_buffer));
    assert(written > 0);

    mg_net_entity_snapshot_t client_received[4];
    uint32_t rec_count = mg_net_replicate_deserialize(client_rep, packet_buffer, written, client_received, 4);
    assert(rec_count == 2);
    assert(client_received[0].net_id == 101);
    assert(client_received[0].x == 12.5f);
    assert(client_received[1].net_id == 102);
    printf("PASS: Network Entity Replication verified (%u entities synchronized)\n", rec_count);

    mg_net_replicator_destroy(server_rep);
    mg_net_replicator_destroy(client_rep);

    /* 3. Test Fullscreen Post-Processing Stack (ACES Tone Mapping + Bloom) */
    uint32_t test_framebuffer[4] = {
        0xFF804020, /* Low light */
        0xFFFFFFFF, /* Super bright */
        0xFFFF8000, /* Orange flare */
        0xFF00FF00  /* Bright green */
    };

    mg_postprocess_config_t pp_config = {
        .enable_bloom = true,
        .bloom_threshold = 0.7f,
        .bloom_intensity = 0.5f,
        .enable_tonemapping = true,
        .exposure = 1.2f,
        .enable_fxaa = true
    };

    mg_postprocess_apply(test_framebuffer, 2, 2, &pp_config);
    printf("PASS: Post-Processing Stack applied (Processed pixel 1: 0x%08X)\n", test_framebuffer[1]);

    printf("PASS: Example 10: Async Asset Streaming, Network Replication, and Post-Processing completed successfully\n");
    return 0;
}
