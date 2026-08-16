/**
 * @file mg_fs.c
 * @brief Cross-platform filesystem operations implementation.
 */
#include "mg/pal/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <io.h>
    #define F_OK 0
    #define access _access
#else
    #include <unistd.h>
#endif

struct mg_fs_watcher {
    char path[1024];
    uint64_t last_mtime;
};

void* mg_fs_read_file(const char* path, size_t* out_size) {
    if (!path) return NULL;
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    fseek(f, 0, SEEK_SET);

    void* buffer = malloc((size_t)size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, (size_t)size, f);
    fclose(f);

    if (read_bytes != (size_t)size) {
        free(buffer);
        return NULL;
    }

    if (out_size) {
        *out_size = (size_t)size;
    }
    return buffer;
}

char* mg_fs_read_text(const char* path) {
    if (!path) return NULL;
    size_t size = 0;
    char* data = (char*)mg_fs_read_file(path, &size);
    if (!data) return NULL;

    char* text = (char*)realloc(data, size + 1);
    if (!text) {
        free(data);
        return NULL;
    }
    text[size] = '\0';
    return text;
}

bool mg_fs_write_file(const char* path, const void* data, size_t size) {
    if (!path || (!data && size > 0)) return false;
    FILE* f = fopen(path, "wb");
    if (!f) return false;

    if (size > 0) {
        size_t written = fwrite(data, 1, size, f);
        fclose(f);
        return written == size;
    }
    fclose(f);
    return true;
}

bool mg_fs_file_exists(const char* path) {
    if (!path) return false;
#if defined(_WIN32)
    return access(path, F_OK) == 0;
#else
    return access(path, F_OK) == 0;
#endif
}

uint64_t mg_fs_get_modified_time(const char* path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
#if defined(__linux__) || defined(__ANDROID__)
    return ((uint64_t)st.st_mtim.tv_sec * 1000000000ULL) + (uint64_t)st.st_mtim.tv_nsec;
#elif defined(__APPLE__)
    return ((uint64_t)st.st_mtimespec.tv_sec * 1000000000ULL) + (uint64_t)st.st_mtimespec.tv_nsec;
#else
    return (uint64_t)st.st_mtime * 1000000000ULL;
#endif
}

bool mg_fs_normalize_path(char* out_buf, size_t out_buf_size, const char* path) {
    if (!out_buf || out_buf_size == 0 || !path) return false;

    size_t in_len = strlen(path);
    if (in_len >= out_buf_size) return false;

    /* Copy and convert backslashes to forward slashes */
    size_t out_idx = 0;
    for (size_t i = 0; i < in_len; ++i) {
        char c = (path[i] == '\\') ? '/' : path[i];

        /* Deduplicate adjacent slashes */
        if (c == '/' && out_idx > 0 && out_buf[out_idx - 1] == '/') {
            continue;
        }

        out_buf[out_idx++] = c;
    }
    out_buf[out_idx] = '\0';
    return true;
}

mg_fs_watcher_t* mg_fs_watcher_create(const char* file_path) {
    if (!file_path) return NULL;
    mg_fs_watcher_t* w = (mg_fs_watcher_t*)malloc(sizeof(mg_fs_watcher_t));
    if (!w) return NULL;

    strncpy(w->path, file_path, sizeof(w->path) - 1);
    w->path[sizeof(w->path) - 1] = '\0';
    w->last_mtime = mg_fs_get_modified_time(file_path);
    return w;
}

void mg_fs_watcher_destroy(mg_fs_watcher_t* watcher) {
    free(watcher);
}

bool mg_fs_watcher_poll(mg_fs_watcher_t* watcher) {
    if (!watcher) return false;
    uint64_t mtime = mg_fs_get_modified_time(watcher->path);
    if (mtime > watcher->last_mtime) {
        watcher->last_mtime = mtime;
        return true;
    }
    return false;
}
