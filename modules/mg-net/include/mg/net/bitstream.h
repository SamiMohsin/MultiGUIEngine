/**
 * @file bitstream.h
 * @brief MultiGUIEngine Arbitrary Bitstream Serialization & Network Delta Compression.
 */
#ifndef MG_NET_BITSTREAM_H
#define MG_NET_BITSTREAM_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_bitstream {
    uint8_t* buffer;
    size_t capacity_bytes;
    size_t bit_cursor;
} mg_bitstream_t;

/**
 * @brief Initializes a bitstream with a memory buffer.
 */
void mg_bitstream_init(mg_bitstream_t* bs, uint8_t* buffer, size_t capacity_bytes);

/**
 * @brief Writes up to 32 bits into the stream.
 */
void mg_bitstream_write_bits(mg_bitstream_t* bs, uint32_t value, uint32_t bit_count);

/**
 * @brief Reads up to 32 bits from the stream.
 */
uint32_t mg_bitstream_read_bits(mg_bitstream_t* bs, uint32_t bit_count);

/**
 * @brief Writes a fixed-point quantized float into the stream.
 */
void mg_bitstream_write_quantized_float(
    mg_bitstream_t* bs,
    float value,
    float min_val,
    float max_val,
    uint32_t bit_count
);

/**
 * @brief Reads and dequantizes a float from the stream.
 */
float mg_bitstream_read_quantized_float(
    mg_bitstream_t* bs,
    float min_val,
    float max_val,
    uint32_t bit_count
);

/**
 * @brief Delta-compresses a 3D position vector against a baseline snapshot.
 * @return Total bytes written.
 */
size_t mg_net_compress_entity_delta(
    const mg_vec3_t* current_pos,
    const mg_vec3_t* baseline_pos,
    uint8_t* out_buffer,
    size_t max_out_bytes
);

/**
 * @brief Decompresses a delta-encoded 3D position vector.
 */
bool mg_net_decompress_entity_delta(
    const uint8_t* in_buffer,
    size_t in_bytes,
    const mg_vec3_t* baseline_pos,
    mg_vec3_t* out_pos
);

#ifdef __cplusplus
}
#endif

#endif /* MG_NET_BITSTREAM_H */
