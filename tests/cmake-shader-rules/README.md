# Isolated shader compilation and embedding test

This fixture uses a simple replacement for shaderc, checking the CMake
build graph and resource embedding without the BGFX compiler or graphics SDK.

Make `fake_shaderc.sh` executable, then configure/build/test twice:
once in the default legacy mode and once with
`-DVISAGE_USE_CUSTOM_RULES=ON` on CMake 4.5+.

It verifies that distinct vertex and fragment shaders produce output,
that the outputs are embedded into C++, and that downstream code links.
It **does not** validate the real shader compiler's output formats or
platform-specific graphics integration.
