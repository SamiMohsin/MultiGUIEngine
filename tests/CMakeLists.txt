# Phase 0: only mg-pal has a test target. Each module adds its own
# tests/unit/<module>/CMakeLists.txt as it lands, per
# .agent-skills/04_CODE_ANALYSIS_PROTOCOL.md Step 5.
add_subdirectory(unit/mg-pal)
add_subdirectory(unit/mg-alloc)
add_subdirectory(unit/mg-math)
add_subdirectory(unit/mg-rx)
add_subdirectory(unit/mg-render)
add_subdirectory(unit/mg-scene)
