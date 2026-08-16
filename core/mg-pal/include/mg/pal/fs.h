/**
 * @file fs.h
 * @brief Filesystem I/O, path normalization, and change-watching utilities.
 */
#ifndef MG_PAL_FS_H
#define MG_PAL_FS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque filesystem watcher handle */
typedef struct mg_fs_watcher mg_fs_watcher_t;

/**
 * @brief Reads an entire binary file into memory.
 * @param path Path to file.
 * @param out_size Pointer receiving byte size of read data.
 * @return Pointer to buffer containing file content (must be freed by caller), or NULL on error.
 */
void* mg_fs_read_file(const char* path, size_t* out_size);

/**
 * @brief Reads an entire text file into a null-terminated string.
 * @param path Path to file.
 * @return Pointer to null-terminated string (must be freed by caller), or NULL on error.
 */
char* mg_fs_read_text(const char* path);

/**
 * @brief Writes buffer content to a file, replacing existing contents.
 * @param path Path to destination file.
 * @param data Data buffer to write.
 * @param size Number of bytes to write.
 * @return True on success, false on error.
 */
bool mg_fs_write_file(const char* path, const void* data, size_t size);

/**
 * @brief Checks if a file or directory exists at the given path.
 * @param path Path to check.
 * @return True if exists, false otherwise.
 */
bool mg_fs_file_exists(const char* path);

/**
 * @brief Retrieves last modification timestamp of a file in nanoseconds.
 * @param path Path to file.
 * @return Modification timestamp in ns, or 0 on failure.
 */
uint64_t mg_fs_get_modified_time(const char* path);

/**
 * @brief Normalizes a file path (converts backslashes to forward slashes, resolves relative dot segments).
 * @param out_buf Output buffer for normalized path.
 * @param out_buf_size Capacity of output buffer.
 * @param path Input path string.
 * @return True if normalized path fits in buffer, false on truncation/error.
 */
bool mg_fs_normalize_path(char* out_buf, size_t out_buf_size, const char* path);

/**
 * @brief Creates a filesystem change watcher for a specific file.
 * @param file_path Path to file to monitor.
 * @return Pointer to watcher handle, or NULL on error.
 */
mg_fs_watcher_t* mg_fs_watcher_create(const char* file_path);

/**
 * @brief Destroys a filesystem watcher.
 * @param watcher Pointer to watcher. Safe to pass NULL.
 */
void mg_fs_watcher_destroy(mg_fs_watcher_t* watcher);

/**
 * @brief Polls the watcher for modifications.
 * @param watcher Pointer to watcher.
 * @return True if the monitored file was modified since last check.
 */
bool mg_fs_watcher_poll(mg_fs_watcher_t* watcher);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_FS_H */
