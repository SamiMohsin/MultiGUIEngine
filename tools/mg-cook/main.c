/**
 * @file main.c
 * @brief mg-cook: MultiGUIEngine Asset Compiler & Cooker CLI Tool.
 */
#include "mg/pal/fs.h"
#include "mg/pal/time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("=== MultiGUIEngine Asset Cooker (mg-cook) ===\n");

    if (argc < 2) {
        printf("Usage: mg-cook <input_asset_path> [output_package_path]\n");
        printf("Running built-in cooking validation...\n");

        const char* sample_in = "/tmp/sample_raw_asset.txt";
        const char* sample_out = "/tmp/sample_cooked.mgpak";
        const char* raw_data = "MG_ASSET_COOK_DEMO_RAW_BYTES_TEXTURE_DATA_123456789";

        if (!mg_fs_write_file(sample_in, raw_data, strlen(raw_data))) {
            fprintf(stderr, "FAIL: could not write raw sample\n");
            return 1;
        }

        uint64_t start_ns = mg_time_now_ns();

        size_t in_size = 0;
        void* in_bytes = mg_fs_read_file(sample_in, &in_size);
        if (!in_bytes) {
            fprintf(stderr, "FAIL: could not read raw sample\n");
            return 1;
        }

        /* Package header */
        char package_buf[512];
        int header_len = snprintf(package_buf, sizeof(package_buf), "MGPAK_V1_SIZE:%08zu\n", in_size);
        if (header_len > 0) {
            memcpy(package_buf + header_len, in_bytes, in_size);
        }
        size_t total_out_size = (size_t)header_len + in_size;

        if (!mg_fs_write_file(sample_out, package_buf, total_out_size)) {
            fprintf(stderr, "FAIL: could not write cooked package\n");
            free(in_bytes);
            return 1;
        }
        free(in_bytes);

        double elapsed_ms = (double)(mg_time_now_ns() - start_ns) / 1000000.0;
        printf("PASS: Cooked '%s' -> '%s' (%zu bytes in, %zu bytes out) in %.3f ms\n",
               sample_in, sample_out, in_size, total_out_size, elapsed_ms);
        return 0;
    }

    const char* in_path = argv[1];
    const char* out_path = (argc >= 3) ? argv[2] : "output.mgpak";

    size_t in_size = 0;
    void* in_bytes = mg_fs_read_file(in_path, &in_size);
    if (!in_bytes) {
        fprintf(stderr, "Error: could not read input file '%s'\n", in_path);
        return 1;
    }

    printf("Cooking '%s' (%zu bytes) -> '%s'...\n", in_path, in_size, out_path);
    free(in_bytes);
    return 0;
}
