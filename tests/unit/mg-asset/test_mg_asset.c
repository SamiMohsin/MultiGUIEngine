/**
 * @file test_mg_asset.c
 * @brief Comprehensive headless unit tests for mg-asset.
 */
#include "mg/asset/asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_asset_loader_and_refcount(void) {
    printf("Testing asset loader and reference counting...\n");

    const char* tmp_file = "/tmp/mg_test_asset.txt";
    const char* content = "Hello MultiGUIEngine Asset System!";
    if (!mg_fs_write_file(tmp_file, content, strlen(content))) {
        fprintf(stderr, "FAIL: could not write temp asset file\n");
        return 1;
    }

    mg_asset_manager_t* mgr = mg_asset_manager_create("");
    if (!mgr) return 1;

    mg_asset_handle_t h = mg_asset_load(mgr, tmp_file, MG_ASSET_TYPE_TEXT);
    if (h == MG_INVALID_ASSET) {
        fprintf(stderr, "FAIL: mg_asset_load returned invalid handle\n");
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    const mg_asset_info_t* info = mg_asset_get(mgr, h);
    if (!info || info->state != MG_ASSET_STATE_LOADED || !info->data) {
        fprintf(stderr, "FAIL: asset info not in LOADED state\n");
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    if (info->ref_count != 1 || strncmp((const char*)info->data, content, strlen(content)) != 0) {
        fprintf(stderr, "FAIL: asset content mismatch\n");
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    mg_asset_retain(mgr, h);
    if (info->ref_count != 2) {
        fprintf(stderr, "FAIL: ref_count after retain is %d != 2\n", info->ref_count);
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    mg_asset_release(mgr, h);
    if (info->ref_count != 1) {
        fprintf(stderr, "FAIL: ref_count after release is %d != 1\n", info->ref_count);
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    mg_asset_release(mgr, h); /* Drops to 0, frees slot */
    const mg_asset_info_t* dead_info = mg_asset_get(mgr, h);
    if (dead_info != NULL) {
        fprintf(stderr, "FAIL: released asset should return NULL\n");
        mg_asset_manager_destroy(mgr);
        return 1;
    }

    mg_asset_manager_destroy(mgr);
    printf("PASS: asset loader and reference counting\n");
    return 0;
}

int main(void) {
    if (test_asset_loader_and_refcount() != 0) return 1;

    printf("All mg-asset unit tests passed successfully!\n");
    return 0;
}
