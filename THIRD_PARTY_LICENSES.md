# Third-Party Licenses

Every entry vendored under `third_party/` must have a row here before it is used in a build. Enforced by `cmake/LicenseAudit.cmake`. Only MIT, BSD, zlib, Apache-2.0, or Public Domain licenses are accepted — see `.agent-skills/01_ARCHITECTURE_FLOW.md` Section 4.

| Library | Version | License | SPDX | Used by | Justification |
|---|---|---|---|---|---|
| _(none vendored yet — Phase 0 has no third-party code)_ | | | | | |

## Planned dependencies (approved, not yet vendored)

These are pre-approved per `docs/ARCHITECTURE.md` Section 5 and may be added without further sign-off, provided the exact pinned version is recorded here at the time of vendoring:

| Library | License | SPDX | Purpose |
|---|---|---|---|
| GLFW | zlib/libpng | Zlib | windowing + input (`mg-pal`) |
| volk | MIT | MIT | Vulkan function loader (`mg-render`) |
| SPIRV-Cross | Apache-2.0 | Apache-2.0 | shader cross-compilation |
| glslang | BSD-3-Clause/Apache-2.0 | BSD-3-Clause | GLSL→SPIR-V compilation |
| stb_truetype.h / stb_image.h | Public Domain / MIT | MIT | font rasterization, image loading |
| dr_wav.h / dr_mp3.h / dr_flac.h | Public Domain / MIT | MIT | audio decoding (`mg-audio`) |
| wasm3 | MIT | MIT | sandboxed addon runtime (`mg-wasm-host`) |

Any dependency NOT in this table requires a new ADR and explicit human sign-off before vendoring, per `.agent-skills/01_ARCHITECTURE_FLOW.md` Section 4 step 4.
