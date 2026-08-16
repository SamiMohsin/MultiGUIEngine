/**
 * @file mg_bitstream.c
 * @brief MultiGUIEngine Arbitrary Bitstream Serialization & Network Delta Compression implementation.
 */
#include "mg/net/bitstream.h"
#include <string.h>

#define MG_DELTA_RANGE_MIN -10.0f
#define MG_DELTA_RANGE_MAX  10.0f
#define MG_DELTA_BITS       10

void mg_bitstream_init(mg_bitstream_t* bs, uint8_t* buffer, size_t capacity_bytes) {
    if (!bs) return;
    bs->buffer = buffer;
    bs->capacity_bytes = capacity_bytes;
    bs->bit_cursor = 0;
    if (buffer && capacity_bytes > 0) {
        memset(buffer, 0, capacity_bytes);
    }
}

void mg_bitstream_write_bits(mg_bitstream_t* bs, uint32_t value, uint32_t bit_count) {
    if (!bs || !bs->buffer || bit_count == 0 || bit_count > 32) return;

    for (uint32_t i = 0; i < bit_count; ++i) {
        size_t byte_idx = bs->bit_cursor / 8;
        size_t bit_idx = bs->bit_cursor % 8;

        if (byte_idx >= bs->capacity_bytes) return;

        uint8_t bit = (uint8_t)((value >> i) & 1u);
        bs->buffer[byte_idx] |= (uint8_t)(bit << bit_idx);
        bs->bit_cursor++;
    }
}

uint32_t mg_bitstream_read_bits(mg_bitstream_t* bs, uint32_t bit_count) {
    if (!bs || !bs->buffer || bit_count == 0 || bit_count > 32) return 0;

    uint32_t result = 0;
    for (uint32_t i = 0; i < bit_count; ++i) {
        size_t byte_idx = bs->bit_cursor / 8;
        size_t bit_idx = bs->bit_cursor % 8;

        if (byte_idx >= bs->capacity_bytes) break;

        uint32_t bit = (bs->buffer[byte_idx] >> bit_idx) & 1u;
        result |= (bit << i);
        bs->bit_cursor++;
    }
    return result;
}

void mg_bitstream_write_quantized_float(
    mg_bitstream_t* bs,
    float value,
    float min_val,
    float max_val,
    uint32_t bit_count
) {
    if (!bs || bit_count == 0 || bit_count > 32) return;

    float range = max_val - min_val;
    if (range <= 0.0001f) range = 1.0f;

    float norm = (value - min_val) / range;
    if (norm < 0.0f) norm = 0.0f;
    if (norm > 1.0f) norm = 1.0f;

    uint32_t max_int = (bit_count == 32) ? 0xFFFFFFFFu : ((1u << bit_count) - 1u);
    uint32_t quantized = (uint32_t)(norm * (float)max_int + 0.5f);

    mg_bitstream_write_bits(bs, quantized, bit_count);
}

float mg_bitstream_read_quantized_float(
    mg_bitstream_t* bs,
    float min_val,
    float max_val,
    uint32_t bit_count
) {
    if (!bs || bit_count == 0 || bit_count > 32) return min_val;

    uint32_t quantized = mg_bitstream_read_bits(bs, bit_count);
    uint32_t max_int = (bit_count == 32) ? 0xFFFFFFFFu : ((1u << bit_count) - 1u);

    float norm = (float)quantized / (float)max_int;
    return min_val + norm * (max_val - min_val);
}

size_t mg_net_compress_entity_delta(
    const mg_vec3_t* current_pos,
    const mg_vec3_t* baseline_pos,
    uint8_t* out_buffer,
    size_t max_out_bytes
) {
    if (!current_pos || !baseline_pos || !out_buffer || max_out_bytes < 4) return 0;

    mg_bitstream_t bs;
    mg_bitstream_init(&bs, out_buffer, max_out_bytes);

    float dx = current_pos->x - baseline_pos->x;
    float dy = current_pos->y - baseline_pos->y;
    float dz = current_pos->z - baseline_pos->z;

    mg_bitstream_write_quantized_float(&bs, dx, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);
    mg_bitstream_write_quantized_float(&bs, dy, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);
    mg_bitstream_write_quantized_float(&bs, dz, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);

    /* Total bits = 30 bits -> 4 bytes */
    return (bs.bit_cursor + 7) / 8;
}

bool mg_net_decompress_entity_delta(
    const uint8_t* in_buffer,
    size_t in_bytes,
    const mg_vec3_t* baseline_pos,
    mg_vec3_t* out_pos
) {
    if (!in_buffer || in_bytes < 4 || !baseline_pos || !out_pos) return false;

    mg_bitstream_t bs;
    bs.buffer = (uint8_t*)in_buffer;
    bs.capacity_bytes = in_bytes;
    bs.bit_cursor = 0;

    float dx = mg_bitstream_read_quantized_float(&bs, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);
    float dy = mg_bitstream_read_quantized_float(&bs, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);
    float dz = mg_bitstream_read_quantized_float(&bs, MG_DELTA_RANGE_MIN, MG_DELTA_RANGE_MAX, MG_DELTA_BITS);

    out_pos->x = baseline_pos->x + dx;
    out_pos->y = baseline_pos->y + dy;
    out_pos->z = baseline_pos->z + dz;

    return true;
}
