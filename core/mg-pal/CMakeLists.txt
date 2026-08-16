add_library(mg-pal STATIC
    src/mg_pal_stub.c
)

target_include_directories(mg-pal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Per-platform sources are added in Phase 1 once mg_window_t / mg_thread_t
# etc. are implemented for real. Phase 0 ships only a version stub so the
# top-level build has something to configure and CI has something to run.
