/**
 * @file cpu.h
 * @brief Multi-Architecture CPU capability detection and performance tier scaling.
 */
#ifndef MG_PAL_CPU_H
#define MG_PAL_CPU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_cpu_arch {
    MG_ARCH_UNKNOWN = 0,
    MG_ARCH_X86_64,
    MG_ARCH_ARM64,
    MG_ARCH_ARM32,
    MG_ARCH_RISCV64,
    MG_ARCH_WASM32
} mg_cpu_arch_t;

typedef enum mg_simd_ext {
    MG_SIMD_NONE = 0,
    MG_SIMD_SSE2 = 1 << 0,
    MG_SIMD_AVX2 = 1 << 1,
    MG_SIMD_AVX512 = 1 << 2,
    MG_SIMD_NEON = 1 << 3,
    MG_SIMD_SVE = 1 << 4,
    MG_SIMD_RVV = 1 << 5,
    MG_SIMD_WASM128 = 1 << 6
} mg_simd_ext_t;

typedef enum mg_perf_tier {
    MG_PERF_TIER_EMBEDDED_LOW = 0, /**< Low-power MCU/RISC-V/WASM, single-threaded, minimal RAM. */
    MG_PERF_TIER_MOBILE,           /**< Battery-aware, ARM DynamIQ big.LITTLE, thermal throttled. */
    MG_PERF_TIER_DESKTOP_HIGH      /**< High performance x86_64 / ARM64, multi-queue, max memory pool. */
} mg_perf_tier_t;

typedef struct mg_cpu_caps {
    mg_cpu_arch_t arch;
    uint32_t simd_flags;
    uint32_t logical_cores;
    uint32_t physical_cores;
    mg_perf_tier_t recommended_tier;
    const char* arch_name;
} mg_cpu_caps_t;

/**
 * @brief Detects processor architecture and hardware capabilities at runtime.
 */
mg_cpu_caps_t mg_cpu_get_capabilities(void);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_CPU_H */
