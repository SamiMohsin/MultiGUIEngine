/**
 * @file prediction.h
 * @brief MultiGUIEngine Network Client-Side Prediction, Server Reconciliation & Snapshot Interpolation.
 */
#ifndef MG_NET_PREDICTION_H
#define MG_NET_PREDICTION_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_net_input_cmd {
    uint32_t sequence;
    float dt;
    mg_vec3_t move_dir;
    float speed;
} mg_net_input_cmd_t;

typedef struct mg_net_server_snapshot {
    uint32_t tick;
    uint32_t acked_sequence;
    mg_vec3_t position;
    mg_vec3_t velocity;
} mg_net_server_snapshot_t;

/**
 * @brief Applies local client-side prediction for an input command.
 */
mg_vec3_t mg_net_predict_movement(
    const mg_vec3_t* current_pos,
    const mg_net_input_cmd_t* cmd
);

/**
 * @brief Reconciles local position against authoritative server snapshot and replays unacknowledged commands.
 */
mg_vec3_t mg_net_reconcile_state(
    const mg_net_server_snapshot_t* server_snap,
    const mg_net_input_cmd_t* unacked_cmds,
    uint32_t cmd_count
);

/**
 * @brief Interpolates remote proxy entity position between two server snapshots.
 */
mg_vec3_t mg_net_interpolate_snapshots(
    const mg_net_server_snapshot_t* from,
    const mg_net_server_snapshot_t* to,
    float alpha
);

#ifdef __cplusplus
}
#endif

#endif /* MG_NET_PREDICTION_H */
