
function(visage_embed_shaders project include_filename namespace original_shaders)
  file(GLOB_RECURSE SHADER_INCLUDES shaders/*.sh shaders/varying.def.sc)

  set(SHADER_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders)
  set(DX_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders/dx)
  set(METAL_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders/metal)
  set(GLSL_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders/glsl)
  set(SPIRV_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders/spirv)
  set(ESSL_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shaders/essl)

  if(DEFINED VISAGE_SHADERC_EXECUTABLE AND NOT "${VISAGE_SHADERC_EXECUTABLE}" STREQUAL "")
    set(SHADERC "${VISAGE_SHADERC_EXECUTABLE}")
  else()
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
      set(SHADERC "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/bin/win32/shaderc.exe")
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
      set(SHADERC "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/bin/macos/shaderc")
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
      set(SHADERC "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/bin/linux/shaderc")
    endif()
  endif()

  file(MAKE_DIRECTORY ${SHADER_FOLDER})
  file(MAKE_DIRECTORY ${DX_FOLDER})
  file(MAKE_DIRECTORY ${METAL_FOLDER})
  file(MAKE_DIRECTORY ${GLSL_FOLDER})
  file(MAKE_DIRECTORY ${SPIRV_FOLDER})
  file(MAKE_DIRECTORY ${ESSL_FOLDER})

  if (EMSCRIPTEN)
    set(OPENGLES TRUE)
    set(SHADER_PLATFORM "asm.js")
  elseif (WIN32)
    set(SHADER_PLATFORM "windows")
    set(DIRECTX TRUE)
  elseif (APPLE)
    set(SHADER_PLATFORM "osx")
    set(METAL TRUE)
  elseif (UNIX)
    set(LINUX TRUE)
    set(VULKAN TRUE)
    set(SHADER_PLATFORM "linux")
  endif()

  # CMake 4.5 template rules are opt-in until the 4.5 generators have been
  # exercised on the macOS/Windows/Linux CI matrix. The old per-output
  # commands remain the default for downstream projects using CMake >= 3.17.
  if(VISAGE_USE_CUSTOM_RULES)
    if(CMAKE_VERSION VERSION_LESS 4.5)
      message(FATAL_ERROR "VISAGE_USE_CUSTOM_RULES requires CMake 4.5 or later")
    endif()

    if(OPENGLES)
      set(_visage_shader_flavour essl)
      set(_visage_shader_profile 100_es)
      set(_visage_shader_opt -O3)
    elseif(DIRECTX)
      set(_visage_shader_flavour dx)
      set(_visage_shader_profile s_4_0)
      set(_visage_shader_opt -O3)
    elseif(METAL)
      set(_visage_shader_flavour metal)
      set(_visage_shader_profile metal)
      set(_visage_shader_opt)
    elseif(OPENGL)
      set(_visage_shader_flavour glsl)
      set(_visage_shader_profile 120)
      set(_visage_shader_opt)
    elseif(VULKAN)
      set(_visage_shader_flavour spirv)
      set(_visage_shader_profile spirv)
      set(_visage_shader_opt)
    else()
      message(FATAL_ERROR "No shader backend selected for Visage custom rules")
    endif()

    # The rule is instantiated automatically for every file in its typed file
    # set. Keep the old output paths so the embedding pipeline is unchanged.
    set(_visage_output "<CURRENT_BINARY_DIR>/shaders/${_visage_shader_flavour}/<BASE_NAME>")
    add_custom_rule(visage_vertex_shader
      OUTPUT "${_visage_output}"
      COMMAND "${SHADERC}" -i "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders"
              ${_visage_shader_opt}
              -f "<SOURCE>" -o "${_visage_output}"
              --type v --platform "${SHADER_PLATFORM}" -p "${_visage_shader_profile}"
      DEPENDS ${SHADER_INCLUDES}
    )
    add_custom_rule(visage_fragment_shader
      OUTPUT "${_visage_output}"
      COMMAND "${SHADERC}" -i "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders"
              ${_visage_shader_opt}
              -f "<SOURCE>" -o "${_visage_output}"
              --type f --platform "${SHADER_PLATFORM}" -p "${_visage_shader_profile}"
      DEPENDS ${SHADER_INCLUDES}
    )

    set(_visage_vertex_files)
    set(_visage_fragment_files)
    foreach(SHADER IN LISTS original_shaders)
      get_filename_component(SHADER_NAME "${SHADER}" NAME_WE)
      list(APPEND SHADER_BINS "${SHADER_FOLDER}/${_visage_shader_flavour}/${SHADER_NAME}")
      if(SHADER_NAME MATCHES "^fs_")
        list(APPEND _visage_fragment_files "${SHADER}")
      else()
        list(APPEND _visage_vertex_files "${SHADER}")
      endif()
    endforeach()

    # A separate target owns the compilation file sets. Resource embedding
    # consumes their outputs, but must not itself own the shader compilation.
    add_custom_target(VisageShaderCompilation)
    if(_visage_vertex_files)
      target_sources(VisageShaderCompilation PRIVATE
        FILE_SET visage_vertex_files TYPE visage_vertex_shader
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${_visage_vertex_files})
    endif()
    if(_visage_fragment_files)
      target_sources(VisageShaderCompilation PRIVATE
        FILE_SET visage_fragment_files TYPE visage_fragment_shader
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${_visage_fragment_files})
    endif()
  else()
  foreach(SHADER ${original_shaders})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

    set(SHADER_TYPE "v")
    if (${SHADER_NAME} MATCHES fs_*)
      set(SHADER_TYPE "f")
    endif()

    if (OPENGLES)
      set(ESSL_PATH ${ESSL_FOLDER}/${SHADER_NAME})
      list(APPEND SHADER_BINS ${ESSL_PATH})
      add_custom_command(
        OUTPUT ${ESSL_PATH}
        COMMAND ${SHADERC} -i ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders -O3 -f ${SHADER} -o ${ESSL_PATH} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} -p 100_es
        DEPENDS ${SHADER}
      )
    endif()

    if (DIRECTX)
      set(DX_PATH ${DX_FOLDER}/${SHADER_NAME})
      list(APPEND SHADER_BINS ${DX_PATH})
      add_custom_command(
        OUTPUT ${DX_PATH}
        COMMAND ${SHADERC} -i ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders -O3 -f ${SHADER} -o ${DX_PATH} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} -p s_4_0
        DEPENDS ${SHADER}
      )
    endif()

    if (METAL)
      set(METAL_PATH ${METAL_FOLDER}/${SHADER_NAME})
      list(APPEND SHADER_BINS ${METAL_PATH})
      add_custom_command(
        OUTPUT ${METAL_PATH}
        COMMAND ${SHADERC} -i ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders -f ${SHADER} -o ${METAL_PATH} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} -p metal
        DEPENDS ${SHADER}
      )
    endif()

    if (OPENGL)
      set(GLSL_PATH ${GLSL_FOLDER}/${SHADER_NAME})
      list(APPEND SHADER_BINS ${GLSL_PATH})
      add_custom_command(
        OUTPUT ${GLSL_PATH}
        COMMAND ${SHADERC} -i ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders -f ${SHADER} -o ${GLSL_PATH} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} -p 120
        DEPENDS ${SHADER}
      )
    endif()

    if (VULKAN)
      set(SPIRV_PATH ${SPIRV_FOLDER}/${SHADER_NAME})
      list(APPEND SHADER_BINS ${SPIRV_PATH})
      add_custom_command(
        OUTPUT ${SPIRV_PATH}
        COMMAND ${SHADERC} -i ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/shaders -f ${SHADER} -o ${SPIRV_PATH} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} -p spirv
        DEPENDS ${SHADER}
      )
    endif()
  endforeach()
  endif()

  set(SHADER_RESOURCES
    ${SHADER_BINS}
    ${SHADER_INCLUDES}
    ${original_shaders}
  )

  add_embedded_resources(${project} ${include_filename} ${namespace} "${SHADER_RESOURCES}")
  if(VISAGE_USE_CUSTOM_RULES)
    add_dependencies(${project} VisageShaderCompilation)
  endif()
  target_sources(${project} PRIVATE ${original_shaders})
  source_group("Shaders" FILES ${original_shaders})
endfunction()

file(GLOB LIBRARY_SHADERS shaders/[vf]s_*.sc)
file(GLOB FONT_TTF_FILES fonts/*.ttf)
file(GLOB ICON_FILES icons/*.svg)

if (UNIX AND NOT APPLE)
  file(GLOB LINUX_TTF_FILES fonts/linux/*.ttf)
  list(APPEND FONT_TTF_FILES ${LINUX_TTF_FILES})
endif ()

visage_embed_shaders(VisageEmbeddedShaders "shaders.h" "visage::shaders" "${LIBRARY_SHADERS}")
add_embedded_resources(VisageEmbeddedFonts "fonts.h" "visage::fonts" "${FONT_TTF_FILES}")
add_embedded_resources(VisageEmbeddedIcons "icons.h" "visage::icons" "${ICON_FILES}")

add_library(VisageGraphicsEmbeds INTERFACE)

target_link_libraries(VisageGraphicsEmbeds
  INTERFACE
  VisageEmbeddedShaders
  VisageEmbeddedFonts
  VisageEmbeddedIcons
)
target_link_libraries(VisageGraphics PRIVATE VisageGraphicsEmbeds)

set_target_properties(VisageEmbeddedShaders PROPERTIES FOLDER "visage")
set_target_properties(VisageEmbeddedFonts PROPERTIES FOLDER "visage")
set_target_properties(VisageEmbeddedIcons PROPERTIES FOLDER "visage")
