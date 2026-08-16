/**
 * @file test_mg_math.c
 * @brief Headless unit tests for mg-math (vec2, vec3, vec4, mat4, quat).
 */
#include "mg/math/math.h"
#include <stdio.h>
#include <math.h>

static bool mg_approx_eq(float a, float b, float eps) {
    return fabsf(a - b) <= eps;
}

static int test_vec_ops(void) {
    printf("Testing vector operations...\n");

    /* Vec2 */
    mg_vec2_t v2a, v2b, v2out;
    mg_vec2_set(&v2a, 3.0f, 4.0f);
    mg_vec2_set(&v2b, 1.0f, 2.0f);
    if (!mg_approx_eq(mg_vec2_length(&v2a), 5.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec2_length\n");
        return 1;
    }
    mg_vec2_add(&v2out, &v2a, &v2b);
    if (!mg_approx_eq(v2out.x, 4.0f, 1e-5f) || !mg_approx_eq(v2out.y, 6.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec2_add\n");
        return 1;
    }
    mg_vec2_normalize(&v2out, &v2a);
    if (!mg_approx_eq(mg_vec2_length(&v2out), 1.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec2_normalize\n");
        return 1;
    }

    /* Vec3 */
    mg_vec3_t v3a, v3b, v3out;
    mg_vec3_set(&v3a, 1.0f, 0.0f, 0.0f);
    mg_vec3_set(&v3b, 0.0f, 1.0f, 0.0f);
    mg_vec3_cross(&v3out, &v3a, &v3b);
    if (!mg_approx_eq(v3out.x, 0.0f, 1e-5f) || !mg_approx_eq(v3out.y, 0.0f, 1e-5f) || !mg_approx_eq(v3out.z, 1.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec3_cross\n");
        return 1;
    }
    if (!mg_approx_eq(mg_vec3_dot(&v3a, &v3b), 0.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec3_dot orthogonal\n");
        return 1;
    }

    /* Vec4 */
    mg_vec4_t v4a, v4b;
    mg_vec4_set(&v4a, 1.0f, 2.0f, 3.0f, 4.0f);
    mg_vec4_set(&v4b, 2.0f, 0.0f, 1.0f, -1.0f);
    if (!mg_approx_eq(mg_vec4_dot(&v4a, &v4b), 1.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: vec4_dot\n");
        return 1;
    }

    printf("PASS: vector operations\n");
    return 0;
}

static int test_mat4_ops(void) {
    printf("Testing mat4 operations...\n");

    mg_mat4_t id, m_trans, m_scale, m_mul, m_inv;
    mg_mat4_identity(&id);

    /* Transform vector by identity */
    mg_vec4_t v = { 1.0f, 2.0f, 3.0f, 1.0f };
    mg_vec4_t v_out;
    mg_mat4_transform_vec4(&v_out, &id, &v);
    if (!mg_approx_eq(v_out.x, 1.0f, 1e-5f) || !mg_approx_eq(v_out.y, 2.0f, 1e-5f) ||
        !mg_approx_eq(v_out.z, 3.0f, 1e-5f) || !mg_approx_eq(v_out.w, 1.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: mat4 identity transform\n");
        return 1;
    }

    /* Translation and Scaling */
    mg_vec3_t t = { 10.0f, -5.0f, 2.0f };
    mg_mat4_translation(&m_trans, &t);
    mg_vec3_t s = { 2.0f, 3.0f, 4.0f };
    mg_mat4_scaling(&m_scale, &s);
    mg_mat4_mul(&m_mul, &m_trans, &m_scale);

    mg_vec3_t pt = { 1.0f, 1.0f, 1.0f };
    mg_vec3_t pt_out;
    mg_mat4_transform_point(&pt_out, &m_trans, &pt);
    if (!mg_approx_eq(pt_out.x, 11.0f, 1e-5f) || !mg_approx_eq(pt_out.y, -4.0f, 1e-5f) ||
        !mg_approx_eq(pt_out.z, 3.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: mat4 translation\n");
        return 1;
    }

    mg_mat4_transform_point(&pt_out, &m_mul, &pt);
    if (!mg_approx_eq(pt_out.x, 12.0f, 1e-5f) || !mg_approx_eq(pt_out.y, -2.0f, 1e-5f) ||
        !mg_approx_eq(pt_out.z, 6.0f, 1e-5f)) {
        fprintf(stderr, "FAIL: mat4 translation * scale\n");
        return 1;
    }

    /* Invert translation matrix */
    bool inv_ok = mg_mat4_invert(&m_inv, &m_trans);
    if (!inv_ok) {
        fprintf(stderr, "FAIL: mat4 invert failed\n");
        return 1;
    }
    mg_mat4_mul(&m_mul, &m_trans, &m_inv);
    for (int i = 0; i < 16; ++i) {
        float expected = (i % 5 == 0) ? 1.0f : 0.0f;
        if (!mg_approx_eq(m_mul.m[i], expected, 1e-4f)) {
            fprintf(stderr, "FAIL: mat4 * inv(mat4) != identity (elem %d: %f vs %f)\n", i, m_mul.m[i], expected);
            return 1;
        }
    }

    /* Perspective projection test */
    mg_mat4_t pers;
    mg_mat4_perspective(&pers, mg_deg_to_rad(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
    if (pers.m[5] >= 0.0f) {
        fprintf(stderr, "FAIL: perspective Y-down must be negative for Vulkan\n");
        return 1;
    }

    printf("PASS: mat4 operations\n");
    return 0;
}

static int test_quat_ops(void) {
    printf("Testing quaternion operations...\n");

    mg_quat_t q1, q2, q_mul;
    mg_vec3_t axis_z = { 0.0f, 0.0f, 1.0f };
    mg_quat_from_axis_angle(&q1, &axis_z, mg_deg_to_rad(90.0f));

    /* Multiply two 90-degree rotations -> 180-degree rotation */
    mg_quat_mul(&q_mul, &q1, &q1);
    mg_vec3_t p180 = { 1.0f, 0.0f, 0.0f };
    mg_vec3_t p180_rot;
    mg_quat_rotate_vec3(&p180_rot, &q_mul, &p180);
    if (!mg_approx_eq(p180_rot.x, -1.0f, 1e-4f) || !mg_approx_eq(p180_rot.y, 0.0f, 1e-4f)) {
        fprintf(stderr, "FAIL: quat_mul two 90-deg rotations\n");
        return 1;
    }

    /* Rotate point (1, 0, 0) by 90 deg around Z -> (0, 1, 0) */
    mg_vec3_t p = { 1.0f, 0.0f, 0.0f };
    mg_vec3_t p_rot;
    mg_quat_rotate_vec3(&p_rot, &q1, &p);

    if (!mg_approx_eq(p_rot.x, 0.0f, 1e-4f) || !mg_approx_eq(p_rot.y, 1.0f, 1e-4f) ||
        !mg_approx_eq(p_rot.z, 0.0f, 1e-4f)) {
        fprintf(stderr, "FAIL: quat_rotate_vec3 (got %f, %f, %f)\n", p_rot.x, p_rot.y, p_rot.z);
        return 1;
    }

    /* Convert to mat4 and verify equivalence */
    mg_mat4_t m_rot;
    mg_quat_to_mat4(&m_rot, &q1);
    mg_vec3_t p_mat_rot;
    mg_mat4_transform_point(&p_mat_rot, &m_rot, &p);
    if (!mg_approx_eq(p_mat_rot.x, p_rot.x, 1e-4f) || !mg_approx_eq(p_mat_rot.y, p_rot.y, 1e-4f) ||
        !mg_approx_eq(p_mat_rot.z, p_rot.z, 1e-4f)) {
        fprintf(stderr, "FAIL: quat_to_mat4 mismatch with quat_rotate_vec3\n");
        return 1;
    }

    /* Slerp from identity to q1 at t=0.5 -> 45 degrees */
    mg_quat_identity(&q2);
    mg_quat_t q_half;
    mg_quat_slerp(&q_half, &q2, &q1, 0.5f);
    mg_quat_rotate_vec3(&p_rot, &q_half, &p);
    float expected_xy = sqrtf(2.0f) * 0.5f;
    if (!mg_approx_eq(p_rot.x, expected_xy, 1e-4f) || !mg_approx_eq(p_rot.y, expected_xy, 1e-4f)) {
        fprintf(stderr, "FAIL: quat_slerp half rotation\n");
        return 1;
    }

    printf("PASS: quaternion operations\n");
    return 0;
}

int main(void) {
    if (test_vec_ops() != 0) return 1;
    if (test_mat4_ops() != 0) return 1;
    if (test_quat_ops() != 0) return 1;
    printf("All mg-math unit tests passed!\n");
    return 0;
}
