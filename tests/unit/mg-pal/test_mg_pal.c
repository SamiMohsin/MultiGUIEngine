/**
 * @file test_mg_pal.c
 * @brief Comprehensive headless unit tests for mg-pal.
 */
#include "mg/pal/pal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_version(void) {
    printf("Testing mg_pal version...\n");
    const char* v = mg_pal_version_string();
    if (v == NULL || strstr(v, "mg-pal") == NULL) {
        fprintf(stderr, "FAIL: unexpected version string: %s\n", v ? v : "NULL");
        return 1;
    }
    printf("PASS: version string (%s)\n", v);
    return 0;
}

static int test_atomics(void) {
    printf("Testing mg_pal atomics...\n");
    mg_atomic_int32_t val = 10;

    int32_t loaded = mg_atomic_load_i32(&val);
    if (loaded != 10) {
        fprintf(stderr, "FAIL: atomic_load_i32\n");
        return 1;
    }

    mg_atomic_store_i32(&val, 20);
    if (mg_atomic_load_i32(&val) != 20) {
        fprintf(stderr, "FAIL: atomic_store_i32\n");
        return 1;
    }

    int32_t prev = mg_atomic_fetch_add_i32(&val, 5);
    if (prev != 20 || mg_atomic_load_i32(&val) != 25) {
        fprintf(stderr, "FAIL: atomic_fetch_add_i32\n");
        return 1;
    }

    prev = mg_atomic_fetch_sub_i32(&val, 10);
    if (prev != 25 || mg_atomic_load_i32(&val) != 15) {
        fprintf(stderr, "FAIL: atomic_fetch_sub_i32\n");
        return 1;
    }

    int32_t expected = 15;
    bool cas_ok = mg_atomic_cas_i32(&val, &expected, 100);
    if (!cas_ok || mg_atomic_load_i32(&val) != 100) {
        fprintf(stderr, "FAIL: atomic_cas_i32 successful case\n");
        return 1;
    }

    expected = 50; /* Mismatch */
    cas_ok = mg_atomic_cas_i32(&val, &expected, 200);
    if (cas_ok || expected != 100 || mg_atomic_load_i32(&val) != 100) {
        fprintf(stderr, "FAIL: atomic_cas_i32 failed case\n");
        return 1;
    }

    printf("PASS: atomics\n");
    return 0;
}

static int test_time(void) {
    printf("Testing mg_pal time...\n");
    uint64_t t1 = mg_time_now_ns();
    mg_time_sleep_ms(10);
    uint64_t t2 = mg_time_now_ns();

    if (t2 <= t1) {
        fprintf(stderr, "FAIL: time not monotonic (%lu -> %lu)\n", (unsigned long)t1, (unsigned long)t2);
        return 1;
    }

    double elapsed = mg_time_since_sec(t1);
    if (elapsed < 0.005 || elapsed > 1.0) {
        fprintf(stderr, "FAIL: unexpected elapsed time (%f sec)\n", elapsed);
        return 1;
    }

    printf("PASS: time (elapsed: %f sec)\n", elapsed);
    return 0;
}

typedef struct {
    mg_mutex_t* mutex;
    mg_condvar_t* cv;
    bool ready;
    int counter;
} thread_test_ctx_t;

static void worker_thread(void* arg) {
    thread_test_ctx_t* ctx = (thread_test_ctx_t*)arg;
    mg_mutex_lock(ctx->mutex);
    ctx->counter += 100;
    ctx->ready = true;
    mg_condvar_signal(ctx->cv);
    mg_mutex_unlock(ctx->mutex);
}

static int test_thread_and_sync(void) {
    printf("Testing mg_pal threads and synchronization...\n");
    thread_test_ctx_t ctx;
    ctx.mutex = mg_mutex_create();
    ctx.cv = mg_condvar_create();
    ctx.ready = false;
    ctx.counter = 42;

    mg_thread_t* t = mg_thread_spawn(worker_thread, &ctx, "test_worker");
    if (!t) {
        fprintf(stderr, "FAIL: mg_thread_spawn returned NULL\n");
        return 1;
    }

    mg_mutex_lock(ctx.mutex);
    while (!ctx.ready) {
        mg_condvar_wait(ctx.cv, ctx.mutex);
    }
    int result = ctx.counter;
    mg_mutex_unlock(ctx.mutex);

    mg_thread_join(t);
    mg_condvar_destroy(ctx.cv);
    mg_mutex_destroy(ctx.mutex);

    if (result != 142) {
        fprintf(stderr, "FAIL: worker result %d != 142\n", result);
        return 1;
    }

    uint32_t cores = mg_thread_get_hardware_concurrency();
    if (cores == 0) {
        fprintf(stderr, "FAIL: hardware concurrency is 0\n");
        return 1;
    }

    printf("PASS: threads and sync (cores: %u)\n", cores);
    return 0;
}

static int test_fs(void) {
    printf("Testing mg_pal filesystem...\n");
    const char* test_file = "test_pal_fs_temp.txt";
    const char* sample_text = "MultiGUIEngine Filesystem Test\nLine 2";

    bool write_ok = mg_fs_write_file(test_file, sample_text, strlen(sample_text));
    if (!write_ok) {
        fprintf(stderr, "FAIL: mg_fs_write_file failed\n");
        return 1;
    }

    if (!mg_fs_file_exists(test_file)) {
        fprintf(stderr, "FAIL: mg_fs_file_exists reported false for created file\n");
        remove(test_file);
        return 1;
    }

    char* read_back = mg_fs_read_text(test_file);
    if (!read_back || strcmp(read_back, sample_text) != 0) {
        fprintf(stderr, "FAIL: mg_fs_read_text mismatch\n");
        if (read_back) free(read_back);
        remove(test_file);
        return 1;
    }
    free(read_back);

    uint64_t mtime = mg_fs_get_modified_time(test_file);
    if (mtime == 0) {
        fprintf(stderr, "FAIL: mg_fs_get_modified_time returned 0\n");
        remove(test_file);
        return 1;
    }

    /* Test path normalization */
    char norm[256];
    bool norm_ok = mg_fs_normalize_path(norm, sizeof(norm), "foo\\\\bar\\baz//qux");
    if (!norm_ok || strcmp(norm, "foo/bar/baz/qux") != 0) {
        fprintf(stderr, "FAIL: mg_fs_normalize_path (got %s)\n", norm);
        remove(test_file);
        return 1;
    }

    remove(test_file);
    printf("PASS: filesystem\n");
    return 0;
}

static int test_window(void) {
    printf("Testing mg_pal windowing & events...\n");
    mg_window_desc_t desc;
    memset(&desc, 0, sizeof(desc));
    desc.title = "Test Window";
    desc.width = 1280;
    desc.height = 720;
    desc.is_headless = true;

    mg_window_t* w = mg_window_create(&desc);
    if (!w) {
        fprintf(stderr, "FAIL: mg_window_create returned NULL\n");
        return 1;
    }

    int32_t width = 0, height = 0;
    mg_window_get_size(w, &width, &height);
    if (width != 1280 || height != 720) {
        fprintf(stderr, "FAIL: mg_window_get_size mismatch (%d x %d)\n", width, height);
        mg_window_destroy(w);
        return 1;
    }

    /* Inject synthetic events */
    mg_event_t ev_key;
    memset(&ev_key, 0, sizeof(ev_key));
    ev_key.type = MG_EVENT_KEY_DOWN;
    ev_key.data.key.key_code = 65; /* 'A' */

    mg_event_t ev_mouse;
    memset(&ev_mouse, 0, sizeof(ev_mouse));
    ev_mouse.type = MG_EVENT_MOUSE_MOVE;
    ev_mouse.data.mouse_move.x = 100.0f;
    ev_mouse.data.mouse_move.y = 200.0f;

    mg_window_inject_event(w, &ev_key);
    mg_window_inject_event(w, &ev_mouse);

    mg_event_t popped;
    bool pop1 = mg_window_pop_event(w, &popped);
    if (!pop1 || popped.type != MG_EVENT_KEY_DOWN || popped.data.key.key_code != 65) {
        fprintf(stderr, "FAIL: pop event 1 mismatch\n");
        mg_window_destroy(w);
        return 1;
    }

    bool pop2 = mg_window_pop_event(w, &popped);
    if (!pop2 || popped.type != MG_EVENT_MOUSE_MOVE || popped.data.mouse_move.x != 100.0f) {
        fprintf(stderr, "FAIL: pop event 2 mismatch\n");
        mg_window_destroy(w);
        return 1;
    }

    bool pop3 = mg_window_pop_event(w, &popped);
    if (pop3) {
        fprintf(stderr, "FAIL: event queue should be empty\n");
        mg_window_destroy(w);
        return 1;
    }

    mg_window_destroy(w);
    printf("PASS: windowing & events\n");
    return 0;
}

int main(void) {
    if (test_version() != 0) return 1;
    if (test_atomics() != 0) return 1;
    if (test_time() != 0) return 1;
    if (test_thread_and_sync() != 0) return 1;
    if (test_fs() != 0) return 1;
    if (test_window() != 0) return 1;

    printf("All mg-pal unit tests passed successfully!\n");
    return 0;
}
