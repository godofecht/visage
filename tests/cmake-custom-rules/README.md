# Visage custom-rule embedding smoke test

```sh
cmake -S tests/cmake-custom-rules -B /tmp/visage-embed-legacy
cmake --build /tmp/visage-embed-legacy
ctest --test-dir /tmp/visage-embed-legacy --output-on-failure
```

On **CMake 4.5 or newer**, reconfigure in a fresh build directory with
`-DVISAGE_USE_CUSTOM_RULES=ON`. That tests the new template-rule path
against the same embedded bytes. The default remains OFF, supporting the
older CMake minimum of the main Visage project.

This isolated fixture tests resource generation only; the platform-specific
shaderc pipeline requires platform toolchains and Visage's graphics dependencies.
