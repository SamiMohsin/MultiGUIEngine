/**
 * @file mg_prediction.c
 * @brief MultiGUIEngine Network Client-Side Prediction, Server Reconciliation & Snapshot Interpolation implementation.
 */
#include "mg/net/prediction.h"

mg_vec3_t mg_net_predict_movement(
    const mg_vec3_t* current_pos,
    const mg_net_input_cmd_t* cmd
) {
    if (!current_pos) return (mg_vec3_t){ 0.0f, 0.0f, 0.0f };
    if (!cmd) return *current_pos;

    return (mg_vec3_t){
        current_pos->x + cmd->move_dir.x * cmd->speed * cmd->dt,
        current_pos->y + cmd->move_dir.y * cmd->speed * cmd->dt,
        current_pos->z + cmd->move_dir.z * cmd->speed * cmd->dt
    };
}

mg_vec3_t mg_net_reconcile_state(
    const mg_net_server_snapshot_t* server_snap,
    const mg_net_input_cmd_t* unacked_cmds,
    uint32_t cmd_count
) {
    if (!server_snap) return (mg_vec3_t){ 0.0f, 0.0f, 0.0f };

    mg_vec3_t reconciled_pos = server_snap->position;

    if (!unacked_cmds || cmd_count == 0) {
        return reconciled_pos;
    }

    for (uint32_t i = 0; i < cmd_count; ++i) {
        if (unacked_cmds[i].sequence > server_snap->acked_sequence) {
            reconciled_pos = mg_net_predict_movement(&reconciled_pos, &unacked_cmds[i]);
        }
    }

    return reconciled_pos;
}

mg_vec3_t mg_net_interpolate_snapshots(
    const mg_net_server_snapshot_t* from,
    const mg_net_server_snapshot_t* to,
    float alpha
) {
    if (!from && !to) return (mg_vec3_t){ 0.0f, 0.0f, 0.0f };
    if (!from) return to->position;
    if (!to) return from->position;

    if (alpha <= 0.0f) return from->position;
    if (alpha >= 1.0f) return to->position;

    return (mg_vec3_t){
        from->position.x + (to->position.x - from->position.x) * alpha,
        from->position.y + (to->position.y - from->position.y) * alpha,
        from->position.z + (to->position.z - from->position.z) * alpha
    };
}
