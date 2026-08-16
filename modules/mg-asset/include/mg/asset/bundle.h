/**
 * @file bundle.h
 * @brief MultiGUIEngine Compressed Asset Bundles & Live Hot-Reload Watcher.
 */
#ifndef MG_ASSET_BUNDLE_H
#define MG_ASSET_BUNDLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_BUNDLE_MAX_FILES 32

typedef struct mg_bundle_entry {
    char path[128];
    uint32_t offset;
    uint32_t size;
    uint32_t crc32;
} mg_bundle_entry_t;

typedef struct mg_asset_bundle {
    uint32_t file_count;
    mg_bundle_entry_t entries[MG_BUNDLE_MAX_FILES];
    uint8_t* payload_data;
    size_t payload_size;
} mg_asset_bundle_t;

typedef void (*mg_asset_reload_cb_t)(const char* path, void* user_data);

typedef struct mg_asset_watch_entry {
    char path[128];
    uint32_t last_crc;
    mg_asset_reload_cb_t callback;
    void* user_data;
} mg_asset_watch_entry_t;

typedef struct mg_asset_watcher {
    mg_asset_watch_entry_t watches[MG_BUNDLE_MAX_FILES];
    uint32_t watch_count;
} mg_asset_watcher_t;

/**
 * @brief Computes IEEE 802.3 CRC32 checksum for a buffer.
 */
uint32_t mg_crc32(const void* data, size_t length);

/**
 * @brief Creates an in-memory asset bundle.
 */
mg_asset_bundle_t* mg_asset_bundle_create(void);

/**
 * @brief Destroys an asset bundle and frees its payload data.
 */
void mg_asset_bundle_destroy(mg_asset_bundle_t* bundle);

/**
 * @brief Adds a file buffer to an asset bundle.
 */
bool mg_asset_bundle_add(
    mg_asset_bundle_t* bundle,
    const char* path,
    const void* data,
    size_t size
);

/**
 * @brief Extracts an asset from a bundle by its path name.
 */
const void* mg_asset_bundle_extract(
    const mg_asset_bundle_t* bundle,
    const char* path,
    size_t* out_size
);

/**
 * @brief Initializes an asset hot-reload watcher.
 */
void mg_asset_watcher_init(mg_asset_watcher_t* watcher);

/**
 * @brief Registers an asset path for hot-reload change notifications.
 */
bool mg_asset_watcher_add(
    mg_asset_watcher_t* watcher,
    const char* path,
    uint32_t initial_crc,
    mg_asset_reload_cb_t callback,
    void* user_data
);

/**
 * @brief Checks an updated buffer against the watched asset's CRC32 and triggers callback if modified.
 */
bool mg_asset_watcher_check(
    mg_asset_watcher_t* watcher,
    const char* path,
    const void* current_data,
    size_t current_size
);

#ifdef __cplusplus
}
#endif

#endif /* MG_ASSET_BUNDLE_H */
