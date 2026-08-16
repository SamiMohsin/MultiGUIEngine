/**
 * mg/pal/version.h
 *
 * Phase 0 stub. Real windowing/thread/fs/time/atomic APIs land in Phase 1
 * per docs/ARCHITECTURE.md Section 11 and 6.1. This header exists so the
 * build system, CI, and headless test harness all have something concrete
 * to compile and link against during Phase 0.
 */
#ifndef MG_PAL_VERSION_H
#define MG_PAL_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define MG_PAL_VERSION_MAJOR 0
#define MG_PAL_VERSION_MINOR 1
#define MG_PAL_VERSION_PATCH 0

/**
 * mg_pal_version_string
 *
 * Returns a static, null-terminated string describing the mg-pal build,
 * e.g. "mg-pal 0.1.0". Ownership: caller does not free the returned
 * pointer; it is statically allocated for the lifetime of the process.
 */
const char* mg_pal_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_VERSION_H */
