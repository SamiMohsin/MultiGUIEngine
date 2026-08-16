/**
 * @file main.c
 * @brief Example 21: Client-Side Prediction, Server Reconciliation & Snapshot Interpolation.
 */
#include "mg/net/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 21: Network Client Prediction & Server Reconciliation ===\n");

    /* 1. Initial Local Client State */
    mg_vec3_t client_pos = { 0.0f, 0.0f, 0.0f };

    /* 2. Client-Side Input Commands (Generated over 3 frames) */
    mg_net_input_cmd_t cmd_history[3] = {
        { .sequence = 1, .dt = 0.016f, .move_dir = { 1.0f, 0.0f, 0.0f }, .speed = 100.0f },
        { .sequence = 2, .dt = 0.016f, .move_dir = { 1.0f, 0.0f, 0.0f }, .speed = 100.0f },
        { .sequence = 3, .dt = 0.016f, .move_dir = { 1.0f, 0.0f, 0.0f }, .speed = 100.0f }
    };

    /* Client predicts all 3 movements locally before server replies */
    client_pos = mg_net_predict_movement(&client_pos, &cmd_history[0]);
    client_pos = mg_net_predict_movement(&client_pos, &cmd_history[1]);
    client_pos = mg_net_predict_movement(&client_pos, &cmd_history[2]);
    printf("PASS: Client-Side Predicted Position: (%.2f, %.2f, %.2f)\n", client_pos.x, client_pos.y, client_pos.z);
    assert(client_pos.x == 4.80f); /* 3 * 0.016 * 100 = 4.80 */

    /* 3. Delayed Server Snapshot arrives (Server only processed up to sequence 1, slight obstacle bump) */
    mg_net_server_snapshot_t server_snap = {
        .tick = 42,
        .acked_sequence = 1,
        .position = { 1.50f, 0.0f, 0.0f }, /* Authoritative corrected pos for seq 1 */
        .velocity = { 100.0f, 0.0f, 0.0f }
    };

    /* 4. Client Reconciles against Server Snapshot & Replays Unacknowledged Commands (seq 2 and 3) */
    mg_vec3_t reconciled_pos = mg_net_reconcile_state(&server_snap, cmd_history, 3);
    printf("PASS: Reconciled & Replayed Position: (%.2f, %.2f, %.2f)\n", reconciled_pos.x, reconciled_pos.y, reconciled_pos.z);
    assert(reconciled_pos.x == 4.70f); /* 1.50 + 2 * (0.016 * 100) = 4.70 */

    /* 5. Snapshot Interpolation for Remote Proxy Entities */
    mg_net_server_snapshot_t snap_a = { .position = { 10.0f, 0.0f, 0.0f } };
    mg_net_server_snapshot_t snap_b = { .position = { 20.0f, 0.0f, 0.0f } };

    mg_vec3_t proxy_pos = mg_net_interpolate_snapshots(&snap_a, &snap_b, 0.5f);
    assert(proxy_pos.x == 15.0f);
    printf("PASS: Remote Proxy Snapshot Interpolation: (%.2f, %.2f, %.2f)\n", proxy_pos.x, proxy_pos.y, proxy_pos.z);

    printf("PASS: Example 21: Client-Side Prediction and Server Reconciliation completed successfully\n");
    return 0;
}
