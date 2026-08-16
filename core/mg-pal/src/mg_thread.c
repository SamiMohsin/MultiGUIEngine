#if !defined(_WIN32)
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/**
 * @file mg_thread.c
 * @brief Cross-platform threading and synchronization implementation.
 */
#include "mg/pal/thread.h"
#include <stdlib.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <process.h>

    struct mg_thread {
        HANDLE handle;
        unsigned int id;
    };

    struct mg_mutex {
        CRITICAL_SECTION cs;
    };

    struct mg_condvar {
        CONDITION_VARIABLE cv;
    };

    typedef struct {
        mg_thread_fn fn;
        void* user_data;
    } win32_thread_arg_t;

    static unsigned __stdcall win32_thread_proc(void* arg) {
        win32_thread_arg_t* targ = (win32_thread_arg_t*)arg;
        mg_thread_fn fn = targ->fn;
        void* user_data = targ->user_data;
        free(targ);
        fn(user_data);
        return 0;
    }

    mg_thread_t* mg_thread_spawn(mg_thread_fn fn, void* user_data, const char* name) {
        (void)name;
        if (!fn) return NULL;
        win32_thread_arg_t* targ = (win32_thread_arg_t*)malloc(sizeof(win32_thread_arg_t));
        if (!targ) return NULL;
        targ->fn = fn;
        targ->user_data = user_data;

        mg_thread_t* t = (mg_thread_t*)malloc(sizeof(mg_thread_t));
        if (!t) {
            free(targ);
            return NULL;
        }

        t->handle = (HANDLE)_beginthreadex(NULL, 0, win32_thread_proc, targ, 0, &t->id);
        if (t->handle == NULL) {
            free(t);
            return NULL;
        }
        return t;
    }

    void mg_thread_join(mg_thread_t* thread) {
        if (!thread) return;
        if (thread->handle) {
            WaitForSingleObject(thread->handle, INFINITE);
            CloseHandle(thread->handle);
        }
        free(thread);
    }

    void mg_thread_yield(void) {
        SwitchToThread();
    }

    uint64_t mg_thread_get_current_id(void) {
        return (uint64_t)GetCurrentThreadId();
    }

    uint32_t mg_thread_get_hardware_concurrency(void) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return (uint32_t)info.dwNumberOfProcessors;
    }

    mg_mutex_t* mg_mutex_create(void) {
        mg_mutex_t* m = (mg_mutex_t*)malloc(sizeof(mg_mutex_t));
        if (!m) return NULL;
        InitializeCriticalSection(&m->cs);
        return m;
    }

    void mg_mutex_destroy(mg_mutex_t* mutex) {
        if (!mutex) return;
        DeleteCriticalSection(&mutex->cs);
        free(mutex);
    }

    void mg_mutex_lock(mg_mutex_t* mutex) {
        if (mutex) EnterCriticalSection(&mutex->cs);
    }

    bool mg_mutex_try_lock(mg_mutex_t* mutex) {
        if (!mutex) return false;
        return TryEnterCriticalSection(&mutex->cs) != 0;
    }

    void mg_mutex_unlock(mg_mutex_t* mutex) {
        if (mutex) LeaveCriticalSection(&mutex->cs);
    }

    mg_condvar_t* mg_condvar_create(void) {
        mg_condvar_t* cv = (mg_condvar_t*)malloc(sizeof(mg_condvar_t));
        if (!cv) return NULL;
        InitializeConditionVariable(&cv->cv);
        return cv;
    }

    void mg_condvar_destroy(mg_condvar_t* cv) {
        free(cv);
    }

    void mg_condvar_wait(mg_condvar_t* cv, mg_mutex_t* mutex) {
        if (cv && mutex) {
            SleepConditionVariableCS(&cv->cv, &mutex->cs, INFINITE);
        }
    }

    bool mg_condvar_wait_timeout_ms(mg_condvar_t* cv, mg_mutex_t* mutex, uint32_t timeout_ms) {
        if (!cv || !mutex) return false;
        return SleepConditionVariableCS(&cv->cv, &mutex->cs, timeout_ms) != 0;
    }

    void mg_condvar_signal(mg_condvar_t* cv) {
        if (cv) WakeConditionVariable(&cv->cv);
    }

    void mg_condvar_broadcast(mg_condvar_t* cv) {
        if (cv) WakeAllConditionVariable(&cv->cv);
    }

#else
    /* POSIX (Linux, macOS, Android, iOS) */
    #define _GNU_SOURCE
    #define _POSIX_C_SOURCE 200809L
    #include <pthread.h>
    #include <sched.h>
    #include <unistd.h>
    #include <time.h>
    #include <errno.h>

    struct mg_thread {
        pthread_t handle;
    };

    struct mg_mutex {
        pthread_mutex_t mutex;
    };

    struct mg_condvar {
        pthread_cond_t cond;
    };

    typedef struct {
        mg_thread_fn fn;
        void* user_data;
    } posix_thread_arg_t;

    static void* posix_thread_proc(void* arg) {
        posix_thread_arg_t* targ = (posix_thread_arg_t*)arg;
        mg_thread_fn fn = targ->fn;
        void* user_data = targ->user_data;
        free(targ);
        fn(user_data);
        return NULL;
    }

    mg_thread_t* mg_thread_spawn(mg_thread_fn fn, void* user_data, const char* name) {
        if (!fn) return NULL;
        posix_thread_arg_t* targ = (posix_thread_arg_t*)malloc(sizeof(posix_thread_arg_t));
        if (!targ) return NULL;
        targ->fn = fn;
        targ->user_data = user_data;

        mg_thread_t* t = (mg_thread_t*)malloc(sizeof(mg_thread_t));
        if (!t) {
            free(targ);
            return NULL;
        }

        int res = pthread_create(&t->handle, NULL, posix_thread_proc, targ);
        if (res != 0) {
            free(targ);
            free(t);
            return NULL;
        }

        if (name != NULL) {
#if defined(__linux__)
            pthread_setname_np(t->handle, name);
#elif defined(__APPLE__)
            /* macOS sets thread name from within the thread itself */
            (void)name;
#endif
        }

        return t;
    }

    void mg_thread_join(mg_thread_t* thread) {
        if (!thread) return;
        pthread_join(thread->handle, NULL);
        free(thread);
    }

    void mg_thread_yield(void) {
        sched_yield();
    }

    uint64_t mg_thread_get_current_id(void) {
        return (uint64_t)pthread_self();
    }

    uint32_t mg_thread_get_hardware_concurrency(void) {
        long n = sysconf(_SC_NPROCESSORS_ONLN);
        return (n > 0) ? (uint32_t)n : 1;
    }

    mg_mutex_t* mg_mutex_create(void) {
        mg_mutex_t* m = (mg_mutex_t*)malloc(sizeof(mg_mutex_t));
        if (!m) return NULL;
        pthread_mutex_init(&m->mutex, NULL);
        return m;
    }

    void mg_mutex_destroy(mg_mutex_t* mutex) {
        if (!mutex) return;
        pthread_mutex_destroy(&mutex->mutex);
        free(mutex);
    }

    void mg_mutex_lock(mg_mutex_t* mutex) {
        if (mutex) pthread_mutex_lock(&mutex->mutex);
    }

    bool mg_mutex_try_lock(mg_mutex_t* mutex) {
        if (!mutex) return false;
        return pthread_mutex_trylock(&mutex->mutex) == 0;
    }

    void mg_mutex_unlock(mg_mutex_t* mutex) {
        if (mutex) pthread_mutex_unlock(&mutex->mutex);
    }

    mg_condvar_t* mg_condvar_create(void) {
        mg_condvar_t* cv = (mg_condvar_t*)malloc(sizeof(mg_condvar_t));
        if (!cv) return NULL;
        pthread_cond_init(&cv->cond, NULL);
        return cv;
    }

    void mg_condvar_destroy(mg_condvar_t* cv) {
        if (!cv) return;
        pthread_cond_destroy(&cv->cond);
        free(cv);
    }

    void mg_condvar_wait(mg_condvar_t* cv, mg_mutex_t* mutex) {
        if (cv && mutex) {
            pthread_cond_wait(&cv->cond, &mutex->mutex);
        }
    }

    bool mg_condvar_wait_timeout_ms(mg_condvar_t* cv, mg_mutex_t* mutex, uint32_t timeout_ms) {
        if (!cv || !mutex) return false;
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += (time_t)(timeout_ms / 1000);
        ts.tv_nsec += (long)((timeout_ms % 1000) * 1000000L);
        if (ts.tv_nsec >= 1000000000L) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }
        return pthread_cond_timedwait(&cv->cond, &mutex->mutex, &ts) == 0;
    }

    void mg_condvar_signal(mg_condvar_t* cv) {
        if (cv) pthread_cond_signal(&cv->cond);
    }

    void mg_condvar_broadcast(mg_condvar_t* cv) {
        if (cv) pthread_cond_broadcast(&cv->cond);
    }

#endif
