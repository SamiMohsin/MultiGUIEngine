/**
 * @file mg_bundle.c
 * @brief MultiGUIEngine Compressed Asset Bundles & Live Hot-Reload Watcher implementation.
 */
#include "mg/asset/bundle.h"
#include <stdlib.h>
#include <string.h>

uint32_t mg_crc32(const void* data, size_t length) {
    if (!data || length == 0) return 0;
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

mg_asset_bundle_t* mg_asset_bundle_create(void) {
    return (mg_asset_bundle_t*)calloc(1, sizeof(mg_asset_bundle_t));
}

void mg_asset_bundle_destroy(mg_asset_bundle_t* bundle) {
    if (!bundle) return;
    if (bundle->payload_data) free(bundle->payload_data);
    free(bundle);
}

bool mg_asset_bundle_add(
    mg_asset_bundle_t* bundle,
    const char* path,
    const void* data,
    size_t size
) {
    if (!bundle || !path || !data || size == 0 || bundle->file_count >= MG_BUNDLE_MAX_FILES) {
        return false;
    }

    uint8_t* new_payload = (uint8_t*)realloc(bundle->payload_data, bundle->payload_size + size);
    if (!new_payload) return false;

    bundle->payload_data = new_payload;
    uint32_t offset = (uint32_t)bundle->payload_size;
    memcpy(bundle->payload_data + offset, data, size);
    bundle->payload_size += size;

    mg_bundle_entry_t* entry = &bundle->entries[bundle->file_count++];
    strncpy(entry->path, path, sizeof(entry->path) - 1);
    entry->offset = offset;
    entry->size = (uint32_t)size;
    entry->crc32 = mg_crc32(data, size);

    return true;
}

const void* mg_asset_bundle_extract(
    const mg_asset_bundle_t* bundle,
    const char* path,
    size_t* out_size
) {
    if (!bundle || !path) return NULL;

    for (uint32_t i = 0; i < bundle->file_count; ++i) {
        if (strcmp(bundle->entries[i].path, path) == 0) {
            if (out_size) *out_size = bundle->entries[i].size;
            return bundle->payload_data + bundle->entries[i].offset;
        }
    }
    return NULL;
}

void mg_asset_watcher_init(mg_asset_watcher_t* watcher) {
    if (watcher) memset(watcher, 0, sizeof(mg_asset_watcher_t));
}

bool mg_asset_watcher_add(
    mg_asset_watcher_t* watcher,
    const char* path,
    uint32_t initial_crc,
    mg_asset_reload_cb_t callback,
    void* user_data
) {
    if (!watcher || !path || watcher->watch_count >= MG_BUNDLE_MAX_FILES) return false;

    mg_asset_watch_entry_t* w = &watcher->watches[watcher->watch_count++];
    strncpy(w->path, path, sizeof(w->path) - 1);
    w->last_crc = initial_crc;
    w->callback = callback;
    w->user_data = user_data;
    return true;
}

bool mg_asset_watcher_check(
    mg_asset_watcher_t* watcher,
    const char* path,
    const void* current_data,
    size_t current_size
) {
    if (!watcher || !path || !current_data || current_size == 0) return false;

    for (uint32_t i = 0; i < watcher->watch_count; ++i) {
        if (strcmp(watcher->watches[i].path, path) == 0) {
            uint32_t new_crc = mg_crc32(current_data, current_size);
            if (new_crc != watcher->watches[i].last_crc) {
                watcher->watches[i].last_crc = new_crc;
                if (watcher->watches[i].callback) {
                    watcher->watches[i].callback(path, watcher->watches[i].user_data);
                }
                return true;
            }
            return false;
        }
    }
    return false;
}
