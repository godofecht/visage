cmake_minimum_required(VERSION 3.20)
project(VisageShaderRulesSmoke LANGUAGES CXX)

option(VISAGE_USE_CUSTOM_RULES "Enable CMake 4.5 custom rule templates" OFF)
set(VISAGE_SHADERC_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/fake_shaderc.sh")

# Use a local compiler stub so the test checks the shader build graph,
# generated outputs, and embedding without downloading the graphics SDK.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../visage_file_embed"
                 "${CMAKE_CURRENT_BINARY_DIR}/visage_file_embed")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/empty.cpp" "void visage_graphics_fixture() {}\n")
add_library(VisageGraphics STATIC "${CMAKE_CURRENT_BINARY_DIR}/empty.cpp")
include("${CMAKE_CURRENT_LIST_DIR}/../../visage_graphics/embedded.cmake")

add_executable(shader_smoke main.cpp)
target_link_libraries(shader_smoke PRIVATE VisageEmbeddedShaders)
target_include_directories(shader_smoke PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/VisageEmbeddedShaders_generated")
enable_testing()
add_test(NAME embedded_shader_bytes COMMAND shader_smoke)
