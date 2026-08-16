/**
 * @file mg_time.c
 * @brief High-resolution monotonic timing implementation.
 */
#include "mg/pal/time.h"

#if defined(_WIN32)
    #include <windows.h>
#else
    #define _POSIX_C_SOURCE 200809L
    #include <time.h>
    #include <unistd.h>
#endif

uint64_t mg_time_now_ns(void) {
#if defined(_WIN32)
    static LARGE_INTEGER frequency;
    static BOOL initialized = FALSE;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = TRUE;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
#endif
}

double mg_time_now_sec(void) {
    return (double)mg_time_now_ns() / 1000000000.0;
}

double mg_time_since_sec(uint64_t start_ns) {
    uint64_t now = mg_time_now_ns();
    if (now <= start_ns) {
        return 0.0;
    }
    return (double)(now - start_ns) / 1000000000.0;
}

void mg_time_sleep_ms(uint32_t ms) {
#if defined(_WIN32)
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = (time_t)(ms / 1000);
    ts.tv_nsec = (long)((ms % 1000) * 1000000L);
    nanosleep(&ts, NULL);
#endif
}

void mg_time_sleep_ns(uint64_t ns) {
#if defined(_WIN32)
    Sleep((DWORD)(ns / 1000000ULL));
#else
    struct timespec ts;
    ts.tv_sec = (time_t)(ns / 1000000000ULL);
    ts.tv_nsec = (long)(ns % 1000000000ULL);
    nanosleep(&ts, NULL);
#endif
}
