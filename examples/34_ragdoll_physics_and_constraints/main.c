/**
 * @file main.c
 * @brief Example 34: 3D Articulated Ragdoll Physics, Multi-Body Constraints & Verlet Relaxation.
 */
#include "mg/physics/physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

static float distance(const mg_vec3_t* a, const mg_vec3_t* b) {
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

int main(void) {
    printf("=== Example 34: 3D Ragdoll Physics & Articulated Multi-Body Constraints ===\n");

    /* 1. Initialize Humanoid Ragdoll Structure */
    mg_ragdoll_t ragdoll;
    mg_ragdoll_init(&ragdoll);

    /* Construct Limbs: Head, Torso (Fixed Pin), Left Arm, Right Arm, Left Leg, Right Leg */
    mg_vec3_t torso_pos = { 0.0f, 10.0f, 0.0f };
    int32_t torso = mg_ragdoll_add_body(&ragdoll, &torso_pos, 0.0f, 0.5f); /* Fixed pivot anchor (mass=0) */

    mg_vec3_t head_pos = { 0.0f, 12.0f, 0.0f };
    int32_t head = mg_ragdoll_add_body(&ragdoll, &head_pos, 5.0f, 0.3f);

    mg_vec3_t left_hand_pos = { -3.0f, 10.0f, 0.0f };
    int32_t left_hand = mg_ragdoll_add_body(&ragdoll, &left_hand_pos, 3.0f, 0.25f);

    mg_vec3_t right_hand_pos = { 3.0f, 10.0f, 0.0f };
    int32_t right_hand = mg_ragdoll_add_body(&ragdoll, &right_hand_pos, 3.0f, 0.25f);

    mg_vec3_t left_foot_pos = { -1.5f, 6.0f, 0.0f };
    int32_t left_foot = mg_ragdoll_add_body(&ragdoll, &left_foot_pos, 8.0f, 0.3f);

    mg_vec3_t right_foot_pos = { 1.5f, 6.0f, 0.0f };
    int32_t right_foot = mg_ragdoll_add_body(&ragdoll, &right_foot_pos, 8.0f, 0.3f);

    assert(ragdoll.body_count == 6);
    printf("PASS: Created 6 Humanoid Ragdoll Limbs (Torso, Head, 2 Arms, 2 Legs)\n");

    /* 2. Connect Limbs with Distance Constraints */
    float head_dist = distance(&torso_pos, &head_pos);             /* 2.0m */
    float arm_dist  = distance(&torso_pos, &left_hand_pos);        /* 3.0m */
    float leg_dist  = distance(&torso_pos, &left_foot_pos);        /* ~4.27m */

    mg_ragdoll_add_joint(&ragdoll, torso, head, head_dist);
    mg_ragdoll_add_joint(&ragdoll, torso, left_hand, arm_dist);
    mg_ragdoll_add_joint(&ragdoll, torso, right_hand, arm_dist);
    mg_ragdoll_add_joint(&ragdoll, torso, left_foot, leg_dist);
    mg_ragdoll_add_joint(&ragdoll, torso, right_foot, leg_dist);

    assert(ragdoll.joint_count == 5);
    printf("PASS: Linked 5 Articulated Joints (Neck, Left Shoulder, Right Shoulder, Left Hip, Right Hip)\n");

    /* 3. Simulate 60 Physics Ticks under Gravity */
    mg_vec3_t gravity = { 0.0f, -9.81f, 0.0f };
    float dt = 0.016f;

    for (int frame = 0; frame < 60; ++frame) {
        mg_ragdoll_step(&ragdoll, dt, &gravity);
    }

    /* 4. Validate Joint Length Constraints after Gravity Fall */
    float final_neck_dist = distance(&ragdoll.bodies[torso].position, &ragdoll.bodies[head].position);
    float final_larm_dist = distance(&ragdoll.bodies[torso].position, &ragdoll.bodies[left_hand].position);
    float final_rarm_dist = distance(&ragdoll.bodies[torso].position, &ragdoll.bodies[right_hand].position);
    float final_lleg_dist = distance(&ragdoll.bodies[torso].position, &ragdoll.bodies[left_foot].position);

    printf("PASS: Joint Rest vs Final -> Neck: %.3f / %.3f m, Arm: %.3f / %.3f m, Leg: %.3f / %.3f m\n",
           head_dist, final_neck_dist, arm_dist, final_larm_dist, leg_dist, final_lleg_dist);

    assert(fabsf(final_neck_dist - head_dist) < 0.01f);
    assert(fabsf(final_larm_dist - arm_dist) < 0.01f);
    assert(fabsf(final_rarm_dist - arm_dist) < 0.01f);
    assert(fabsf(final_lleg_dist - leg_dist) < 0.01f);

    /* Ensure limbs swung downward under gravity */
    assert(ragdoll.bodies[left_hand].position.y < left_hand_pos.y);
    assert(ragdoll.bodies[left_foot].position.y < left_foot_pos.y);

    printf("PASS: Example 34: 3D Ragdoll Physics & Constraints completed successfully\n");
    return 0;
}
