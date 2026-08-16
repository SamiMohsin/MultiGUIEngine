/**
 * Headless test for mg-pal Phase 0 stub.
 * Runs with no window, no GPU, no display server — CI-safe.
 */
#include "mg/pal/version.h"
#include <string.h>
#include <stdio.h>

int main(void) {
    const char* v = mg_pal_version_string();

    if (v == NULL) {
        fprintf(stderr, "FAIL: mg_pal_version_string returned NULL\n");
        return 1;
    }

    if (strstr(v, "mg-pal") == NULL) {
        fprintf(stderr, "FAIL: unexpected version string: %s\n", v);
        return 1;
    }

    printf("PASS: %s\n", v);
    return 0;
}
