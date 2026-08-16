/**
 * @file mg_cpu.c
 * @brief Multi-Architecture CPU capability detection implementation.
 */
#include "mg/pal/cpu.h"
#include "mg/pal/thread.h"
#include <string.h>

mg_cpu_caps_t mg_cpu_get_capabilities(void) {
    mg_cpu_caps_t caps;
    memset(&caps, 0, sizeof(caps));

    caps.logical_cores = mg_thread_get_hardware_concurrency();
    caps.physical_cores = caps.logical_cores > 1 ? caps.logical_cores / 2 : 1;

#if defined(__x86_64__) || defined(_M_X64)
    caps.arch = MG_ARCH_X86_64;
    caps.arch_name = "x86_64";
    caps.simd_flags |= MG_SIMD_SSE2 | MG_SIMD_AVX2;
    caps.recommended_tier = (caps.logical_cores >= 8) ? MG_PERF_TIER_DESKTOP_HIGH : MG_PERF_TIER_MOBILE;
#elif defined(__aarch64__) || defined(_M_ARM64)
    caps.arch = MG_ARCH_ARM64;
    caps.arch_name = "ARM64";
    caps.simd_flags |= MG_SIMD_NEON;
    caps.recommended_tier = (caps.logical_cores >= 8) ? MG_PERF_TIER_DESKTOP_HIGH : MG_PERF_TIER_MOBILE;
#elif defined(__arm__) || defined(_M_ARM)
    caps.arch = MG_ARCH_ARM32;
    caps.arch_name = "ARM32";
    caps.simd_flags |= MG_SIMD_NEON;
    caps.recommended_tier = MG_PERF_TIER_EMBEDDED_LOW;
#elif defined(__riscv)
    caps.arch = MG_ARCH_RISCV64;
    caps.arch_name = "RISC-V 64";
    caps.simd_flags |= MG_SIMD_NONE;
    caps.recommended_tier = MG_PERF_TIER_EMBEDDED_LOW;
#elif defined(__wasm__)
    caps.arch = MG_ARCH_WASM32;
    caps.arch_name = "WebAssembly (WASM32)";
    caps.simd_flags |= MG_SIMD_WASM128;
    caps.recommended_tier = MG_PERF_TIER_MOBILE;
#else
    caps.arch = MG_ARCH_UNKNOWN;
    caps.arch_name = "Generic/Unknown";
    caps.simd_flags = MG_SIMD_NONE;
    caps.recommended_tier = MG_PERF_TIER_EMBEDDED_LOW;
#endif

    return caps;
}
