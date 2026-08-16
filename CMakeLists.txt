cmake_minimum_required(VERSION 3.20)
project(MultiGUIEngine C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---------------------------------------------------------------------------
# Build options — see docs/ARCHITECTURE.md Section 10
# ---------------------------------------------------------------------------
option(MG_ENABLE_3D            "Build 3D renderer/physics"                         ON)
option(MG_ENABLE_NET            "Build networking module"                          ON)
option(MG_ENABLE_AI             "Build AI bridge"                                  ON)
option(MG_ENABLE_WASM_ADDONS    "Build WASM addon host"                            ON)
option(MG_HEADLESS_TESTS        "Build headless test harness"                      ON)
option(MG_TARGET_EMBEDDED       "Build embedded profile (flat SoA, no archetypes)" OFF)

# ---------------------------------------------------------------------------
# Warnings-as-errors per docs/CODING_STANDARDS.md
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# License audit — fails the configure step if a non-permissive license
# sneaks into third_party/. See .agent-skills/01_ARCHITECTURE_FLOW.md Section 4.
# ---------------------------------------------------------------------------
include(LicenseAudit)

# ---------------------------------------------------------------------------
# Phase 0: only mg-pal stub is wired in. Every subsequent phase uncomments
# its subdirectory here as it lands, per docs/ARCHITECTURE.md Section 11.
# Do not add a module's add_subdirectory() until that module has a real
# CMakeLists.txt with at least a stub + headless test, per the Definition
# of Done in docs/ARCHITECTURE.md Section 12.
# ---------------------------------------------------------------------------
add_subdirectory(core/mg-pal)
add_subdirectory(core/mg-alloc)
add_subdirectory(core/mg-math)

# --- Phase 2+ (uncomment as each module lands) ---
# add_subdirectory(core/mg-rx)
# add_subdirectory(modules/mg-render)
# add_subdirectory(modules/mg-scene)
# add_subdirectory(modules/mg-physics)
# add_subdirectory(modules/mg-audio)
# add_subdirectory(modules/mg-ui)
# add_subdirectory(modules/mg-input)
# if(MG_ENABLE_NET)
#     add_subdirectory(modules/mg-net)
# endif()
# add_subdirectory(modules/mg-asset)
# add_subdirectory(plugin-host/mg-plugin-abi)
# if(MG_ENABLE_WASM_ADDONS)
#     add_subdirectory(plugin-host/mg-wasm-host)
# endif()
# if(MG_ENABLE_AI)
#     add_subdirectory(ai-bridge/mg-ai-core)
#     add_subdirectory(ai-bridge/mg-ai-tools)
# endif()
# add_subdirectory(tools/mg-cook)
# add_subdirectory(tools/mg-editor)
# add_subdirectory(tools/mg-profiler)

if(MG_HEADLESS_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "MultiGUIEngine Phase 0 configured. See docs/ARCHITECTURE.md for build order.")
