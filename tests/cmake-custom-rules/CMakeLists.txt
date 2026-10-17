cmake_minimum_required(VERSION 3.20)
project(VisageCustomRulesSmoke LANGUAGES CXX)

# Run with -DVISAGE_USE_CUSTOM_RULES=ON on CMake 4.5+ to test the
# source-file template route. OFF exercises the original custom commands.
option(VISAGE_USE_CUSTOM_RULES "Use CMake 4.5 embedding rule templates" OFF)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../visage_file_embed"
                 "${CMAKE_CURRENT_BINARY_DIR}/visage_file_embed")
add_embedded_resources(EmbedFixture "fixture.h" "visage_smoke"
    "${CMAKE_CURRENT_SOURCE_DIR}/sample.txt")

add_executable(embed_smoke main.cpp)
target_link_libraries(embed_smoke PRIVATE EmbedFixture)
target_include_directories(embed_smoke PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/EmbedFixture_generated")

enable_testing()
add_test(NAME embedded_resource_bytes COMMAND embed_smoke)
