# ADR 0004: First Platform to Reach Fully Green CI

## Context
MultiGUIEngine targets Linux, Windows, macOS, Android, iOS, and Embedded devices. Setting development priority requires selecting a primary reference platform for continuous integration and initial module bring-up.

## Decision
We select **Linux x86_64 (glibc, GCC/Clang, Vulkan / Software RHI)** as the **first platform to reach green CI**:
- Linux provides first-class headless testing tooling, native AddressSanitizer/UBSan/Valgrind support, fast compilation, and reliable CI containerization.
- All engine modules will be brought to full green status and verified headless on Linux before extending full multi-backend graphics validation to Windows and macOS.
- Software rendering (`backend_software`) will be the primary reference implementation running in CI headless environments.

## Consequences
- Clean, fast, reproducible development cycles on standard Linux runners.
- Strict compiler compliance with `-Wall -Wextra -Werror` ensures cleaner portability when targeting MSVC/Clang on Windows/macOS.
