# MultiGUIEngine — Coding Standards

## 1. Language Standard and Compilers
- Written in **C99 / C11**.
- Must compile cleanly with zero warnings under `-Wall -Wextra -Werror` on GCC/Clang and `/W4 /WX` on MSVC.
- Hot assembly paths (AVX2 / ARM NEON) must reside in separate files and provide portable scalar C reference implementations as default fallbacks.

## 2. Naming Conventions
- **Prefix:** All public symbols, functions, types, and constants must start with `mg_` or `MG_`.
- **Types:** Typedefs must end with `_t`, e.g., `mg_arena_t`, `mg_vec3_t`, `mg_mat4_t`.
- **Functions:** `snake_case`, e.g., `mg_arena_alloc()`, `mg_mat4_mul()`.
- **Constants & Macros:** `UPPER_SNAKE_CASE`, e.g., `MG_PI`, `MG_DEG2RAD`.
- **Enums:** `mg_enum_name_t`, values formatted as `MG_ENUM_NAME_VALUE`.

## 3. Memory and Allocation Rules
- **No dynamic allocation on hot paths:** Any function called from the per-frame game loop stages (`INPUT`, `FIXED_UPDATE`, `UPDATE`, `LATE_UPDATE`, `RENDER_EXTRACT`, `RENDER`) must never call `malloc`, `free`, `realloc`, `calloc`, or `new`/`delete`.
- **Frame Memory:** Use bump arenas (`mg_arena_t`) allocated at startup and reset each frame (`mg_arena_reset`).
- **Entity & Node Storage:** Use fixed-size block pools (`mg_pool_t`).
- **Thread Communication:** Use lock-free SPSC ring buffers (`mg_ring_buffer_t`).

## 4. Function Signatures & API Conventions
- **Pointer-to-output for math/structs:** Functions that calculate compound structures (e.g. matrices, quaternions, large vectors) take pointers to destination outputs as the first argument (`mg_mat4_t* out, ...`), avoiding ABI struct-return overhead and discrepancies across compilers.
- **Explicit Ownership:** Function docstrings must explicitly document memory ownership and caller responsibilities for all pointer parameters and return values.

## 5. Documentation
- Every public header file must have full doc-comments on each struct, enum, and function definition describing:
  - High-level purpose.
  - Parameter roles and valid ranges.
  - Return values.
  - Ownership / lifetime expectations.

## 6. Testing & CI
- Every module must include headless unit tests in `tests/unit/<module>/`.
- Tests must execute without requiring a window or physical GPU.
