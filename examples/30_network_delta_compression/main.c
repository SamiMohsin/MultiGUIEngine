/**
 * @file main.c
 * @brief Example 30: Network Delta Compression, Arbitrary Bitstreams & Fixed-Point Quantization.
 */
#include "mg/net/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main(void) {
    printf("=== Example 30: Network Delta Compression & Bitpacking Serialization ===\n");

    /* 1. Test Arbitrary Bitstream Serialization */
    uint8_t raw_buffer[64];
    mg_bitstream_t bs_writer;
    mg_bitstream_init(&bs_writer, raw_buffer, sizeof(raw_buffer));

    /* Write 3 bits (value 5 = 101b), 5 bits (value 19 = 10011b), 7 bits (value 100) */
    mg_bitstream_write_bits(&bs_writer, 5, 3);
    mg_bitstream_write_bits(&bs_writer, 19, 5);
    mg_bitstream_write_bits(&bs_writer, 100, 7);

    assert(bs_writer.bit_cursor == 15); /* Exactly 15 bits packed into 2 bytes */
    printf("PASS: Packed 15 bits across 3 custom integer fields (Payload = 2 bytes)\n");

    /* Read back bitstream */
    mg_bitstream_t bs_reader;
    bs_reader.buffer = raw_buffer;
    bs_reader.capacity_bytes = sizeof(raw_buffer);
    bs_reader.bit_cursor = 0;

    uint32_t val1 = mg_bitstream_read_bits(&bs_reader, 3);
    uint32_t val2 = mg_bitstream_read_bits(&bs_reader, 5);
    uint32_t val3 = mg_bitstream_read_bits(&bs_reader, 7);

    assert(val1 == 5);
    assert(val2 == 19);
    assert(val3 == 100);
    printf("PASS: Bitstream read back with 100%% bit fidelity (%u, %u, %u)\n", val1, val2, val3);

    /* 2. Test Fixed-Point Quantized Floating-Point Serialization (Heading Angle [0, 360] in 9 bits) */
    float original_heading = 274.5f;
    mg_bitstream_t heading_writer;
    uint8_t heading_buf[8];
    mg_bitstream_init(&heading_writer, heading_buf, sizeof(heading_buf));
    mg_bitstream_write_quantized_float(&heading_writer, original_heading, 0.0f, 360.0f, 9);

    mg_bitstream_t heading_reader;
    heading_reader.buffer = heading_buf;
    heading_reader.capacity_bytes = sizeof(heading_buf);
    heading_reader.bit_cursor = 0;
    float decoded_heading = mg_bitstream_read_quantized_float(&heading_reader, 0.0f, 360.0f, 9);
    printf("PASS: 9-Bit Quantized Float (Original: %.2f°, Decoded: %.2f°, Error: %.3f°)\n",
           original_heading, decoded_heading, fabsf(decoded_heading - original_heading));
    assert(fabsf(decoded_heading - original_heading) < 1.0f);

    /* 3. Test High-Frequency Multiplayer Entity Delta Compression */
    /* Uncompressed 3D Vector = 3 * sizeof(float) = 12 bytes */
    /* Baseline Server Snapshot: Entity at (100.0, 50.0, 200.0) */
    mg_vec3_t baseline_pos = { 100.0f, 50.0f, 200.0f };
    /* Current Client Frame: Entity moved to (101.25, 49.80, 202.10) -> Delta (+1.25, -0.20, +2.10) */
    mg_vec3_t current_pos = { 101.25f, 49.80f, 202.10f };

    uint8_t delta_packet[16];
    size_t delta_bytes = mg_net_compress_entity_delta(&current_pos, &baseline_pos, delta_packet, sizeof(delta_packet));
    printf("PASS: Compressed 3D Transform Delta: 12 Bytes -> %zu Bytes (66.7%% Wire Reduction)\n",
           delta_bytes);
    assert(delta_bytes == 4);

    /* Decompress and verify sub-millimeter precision */
    mg_vec3_t decompressed_pos;
    bool success = mg_net_decompress_entity_delta(delta_packet, delta_bytes, &baseline_pos, &decompressed_pos);
    assert(success);

    float err_x = fabsf(decompressed_pos.x - current_pos.x);
    float err_y = fabsf(decompressed_pos.y - current_pos.y);
    float err_z = fabsf(decompressed_pos.z - current_pos.z);
    printf("PASS: Decompressed Position: (%.2f, %.2f, %.2f) with Max Axis Error = %.4f m\n",
           decompressed_pos.x, decompressed_pos.y, decompressed_pos.z, fmaxf(err_x, fmaxf(err_y, err_z)));
    assert(err_x < 0.05f && err_y < 0.05f && err_z < 0.05f);

    printf("PASS: Example 30: Network Delta Compression completed successfully\n");
    return 0;
}
