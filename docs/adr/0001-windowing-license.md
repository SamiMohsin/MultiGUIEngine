# ADR 0001: Third-Party License Policy and GLFW (zlib License)

## Context
The engine architecture specification initially states an "MIT-only" policy in several sections, while also noting that permissive dependencies like GLFW (zlib/libpng license) are approved and pre-listed in `THIRD_PARTY_LICENSES.md`.

## Decision
We formally clarify the project license policy to **MIT or Permissive OSI/FSF-approved Licenses**:
- The MultiGUIEngine core and modules are strictly licensed under the **MIT License**.
- Third-party dependencies must use permissive licenses only: **MIT, BSD (2/3-Clause), zlib/libpng, Apache-2.0, or Public Domain (CC0/Unlicense)**.
- Copyleft licenses (GPL, LGPL, AGPL, MPL, etc.) are strictly forbidden in any module, header, or link graph.
- GLFW is licensed under the zlib/libpng license, which is fully permissive, static-link compatible, and introduces no copyleft contamination. GLFW is formally confirmed as the desktop windowing/input library.

## Consequences
- `THIRD_PARTY_LICENSES.md` and `cmake/LicenseAudit.cmake` validate all vendored dependencies against this permissive whitelist.
- The engine can be safely embedded in proprietary commercial projects and open-source applications without reciprocal source disclosure requirements.
