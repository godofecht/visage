add_library(VisageFileEmbedInclude INTERFACE)
target_include_directories(VisageFileEmbedInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

function(add_embedded_resources project include_filename namespace files)
  get_filename_component(current_dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

  set(generated_path ${CMAKE_CURRENT_BINARY_DIR}/${project}_generated)
  set(destination_path ${generated_path}/embedded)
  file(MAKE_DIRECTORY ${generated_path})
  file(MAKE_DIRECTORY ${destination_path})

  set(lookup_file "${destination_path}/embedded_files0.cpp")

  set(source_files)
  list(APPEND source_files "${lookup_file}")

  if(VISAGE_USE_CUSTOM_RULES)
    # Preserve the legacy embedded_filesN.cpp names: the resource header and
    # lookup table expose the exact same symbols in both build modes.
    # Each project gets a distinct rule, including its own namespace/path.
    set(_embed_rule "visage_embed_${project}")
    set(_embed_output "<CURRENT_BINARY_DIR>/${project}_generated/embedded/embedded_files<INDEX>.cpp")
    add_custom_rule(${_embed_rule}
      OUTPUT "${_embed_output}"
      COMMAND "${CMAKE_COMMAND}"
        "-DDEST_FILE=${_embed_output}"
        "-DORIGINAL_FILE=<SOURCE>"
        "-DVAR_NAMESPACE=${namespace}"
        -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/embed_file.cmake"
      DEPENDS "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/embed_file.cmake"
    )
    set(_embed_source_inputs)
    set(_embed_binary_inputs)
    set(index 1)
    foreach(file IN LISTS files)
      # CMake 4.5 source-specific patterns preserve the original numeric index.
      set_property(SOURCE "${file}" PROPERTY "${_embed_rule}_PATTERNS" "INDEX=${index}")
      cmake_path(IS_PREFIX CMAKE_CURRENT_BINARY_DIR "${file}" NORMALIZE _visage_is_binary_input)
      if(_visage_is_binary_input)
        list(APPEND _embed_binary_inputs "${file}")
      else()
        list(APPEND _embed_source_inputs "${file}")
      endif()
      math(EXPR index "${index} + 1")
    endforeach()
  else()
  set(index 1)
  foreach (file IN LISTS files)
    set(embedded_file_name "embedded_files${index}.cpp")
    set(source_file "${destination_path}/${embedded_file_name}")
    list(APPEND source_files ${source_file})
    get_filename_component(original_file_name ${file} NAME)
    add_custom_command(
      OUTPUT ${source_file}
      COMMAND ${CMAKE_COMMAND} -DDEST_FILE=${source_file} -DORIGINAL_FILE=${file} -DVAR_NAMESPACE=${namespace} -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/embed_file.cmake
      DEPENDS ${file}
      COMMENT "Generating ${embedded_file_name} for ${original_file_name}"
    )
    set_source_files_properties(${source_file} PROPERTIES GENERATED TRUE)
    math(EXPR index "${index} + 1")
  endforeach ()
  endif()

  string(REPLACE ";" "\\;" escaped_files "${files}")
  add_custom_command(
    OUTPUT ${destination_path}/${include_filename} ${lookup_file}
    COMMAND ${CMAKE_COMMAND} -DDEST_FILE=${destination_path}/${include_filename} -DFILE_LIST="${escaped_files}"
    -DVAR_NAMESPACE=${namespace} -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/create_embed_header.cmake"
    COMMAND ${CMAKE_COMMAND} -DDEST_FILE=${lookup_file} -DINCLUDE_FILE=${include_filename} -DFILE_LIST="${escaped_files}"
    -DVAR_NAMESPACE=${namespace} -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/create_file_lookup.cmake"
    DEPENDS ${FILE_EMBEDDER_LIB} ${files} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/create_embed_header.cmake"
    COMMENT "Generating C++ for embedded files..."
  )

  set_source_files_properties(${destination_path}/${include_filename} PROPERTIES GENERATED TRUE)
  set_source_files_properties(${lookup_file} PROPERTIES GENERATED TRUE)
  add_library(${project} STATIC ${destination_path}/${include_filename} ${source_files})
  if(VISAGE_USE_CUSTOM_RULES)
    # Keep source and generated input base directories in separate sets.
    # This also works when the binary tree is nested inside the source tree.
    if(_embed_source_inputs)
      target_sources(${project} PRIVATE FILE_SET ${project}_asset_sources
        TYPE ${_embed_rule} BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${_embed_source_inputs})
    endif()
    if(_embed_binary_inputs)
      target_sources(${project} PRIVATE FILE_SET ${project}_asset_binaries
        TYPE ${_embed_rule} BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
        FILES ${_embed_binary_inputs})
    endif()
  endif()
  target_include_directories(${project}
    PUBLIC
    ${generated_path}
  )
  target_link_libraries(${project}
    PUBLIC
    VisageFileEmbedInclude
  )
endfunction()
