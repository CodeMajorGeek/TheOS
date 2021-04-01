function(theos_install_headers target_name)
    file(GLOB_RECURSE headers RELATIVE ${CMAKE_SOURCE_DIR}/Includes/${traget_name} "*.h")
    foreach(header ${headers})
        get_filename_component(subdirectory ${header} DIRECTORY)
        install(FILES ${header} DESTINATION usr/include/${target_name}/${subdirectory})
    endforeach()
endfunction()

function(theos_install_sources target_name)
    file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.c")
    foreach(source ${sources})
        get_filename_component(subdirectory ${source} DIRECTORY)
        install(FILES ${source} DESTINATION usr/src/theos/${target_name}/${subdirectory})
    endforeach()
endfunction()

function(theos_generated_sources target_name)
    if(DEFINED GENERATED_SOURCES)
        set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED 1)
        foreach(generated ${GENERATED_SOURCES})
            get_filename_component(generated_name ${generated} NAME)
            add_dependencies(${target_name} generate_${generated_name})
        endforeach()
    endif()
endfunction()

function(theos_lib target_name fs_name)
    theos_install_headers(${target_name})
    theos_install_sources("Userland/Libraries/${target_name}")
    #add_library(${target_name} SHARED ${SOURCES} ${GENERATED_SOURCES})
    add_library(${target_name} SHARED ${SOURCES} ${GENERATED_SOURCES})
    install(TARGETS ${target_name} DESTINATION usr/lib)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${fs_name})
    theos_generated_sources(${target_name})
endfunction()

function(theos_shared_lib target_name fs_name)
    theos_install_headers(${target_name})
    theos_install_sources("Userland/Libraries/${target_name}")
    add_library(${target_name} SHARED ${SOURCES} ${GENERATED_SOURCES})
    install(TARGETS ${target_name} DESTINATION usr/lib)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${fs_name})
    theos_generated_sources(${target_name})
endfunction()

function(theos_libc target_name fs_name)
    theos_install_headers(${target_name})
    theos_install_sources("Userland/Libraries/LibC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib -fpic")
    add_library(${target_name} SHARED ${SOURCES})
    install(TARGETS ${target_name} DESTINATION usr/lib)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${fs_name})
    target_link_directories(LibC PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    theos_generated_sources(${target_name})
endfunction()

function(theos_libc_static target_name fs_name)
    theos_install_headers(${target_name})
    theos_install_sources("Userland/Libraries/LibC")
    add_library(${target_name} ${SOURCES})
    install(TARGETS ${target_name} ARCHIVE DESTINATION usr/lib)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${fs_name})
    target_link_directories(${target_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    theos_generated_sources(${target_name})
endfunction()

function(theos_bin target_name)
    add_executable(${target_name} ${SOURCES})
    install(TARGETS ${target_name} RUNTIME DESTINATION bin)
    theos_generated_sources(${target_name})
endfunction()

function(theos_app target_name)
    cmake_parse_arguments(THEOS_APP "" "ICON" "" ${ARGN})

    theos_bin("${target_name}")
    set(small_icon "${CMAKE_SOURCE_DIR}/Base/res/icons/16x16/${THEOS_APP_ICON}.png")
    set(medium_icon "${CMAKE_SOURCE_DIR}/Base/res/icons/32x32/${THEOS_APP_ICON}.png")

    if (EXISTS "${small_icon}")
        embed_resource("${target_name}" theosicon_s "${small_icon}")
    else()
        message(WARNING "Missing small app icon: ${small_icon}")
    endif()

    if (EXISTS "${medium_icon}")
        embed_resource("${target_name}" theos_icon_m "${medium_icon}")
    else()
        message(WARNING "Missing medium app icon: ${medium_icon}")
    endif()
endfunction()

function(compile_gml source output string_name)
    set(source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${write_if_different} ${output} ${CMAKE_SOURCE_DIR}/Meta/text-to-cpp-string.sh ${string_name} ${source}
        VERBATIM
        DEPENDS ${CMAKE_SOURCE_DIR}/Meta/text-to-cpp-string.sh
        MAIN_DEPENDENCY ${source}
    )
    get_filename_component(output_name ${output} NAME)
    add_custom_target(generate_${output_name} DEPENDS ${output})
endfunction()


function(compile_ipc source output)
    set(source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${write_if_different} ${output} ${CMAKE_BINARY_DIR}/Userland/DevTools/IPCCompiler/IPCCompiler ${source}
        VERBATIM
        DEPENDS IPCCompiler
        MAIN_DEPENDENCY ${source}
    )
    get_filename_component(output_name ${output} NAME)
    add_custom_target(generate_${output_name} DEPENDS ${output})
endfunction()

function(embed_resource target section file)
    get_filename_component(asm_file "${file}" NAME)
    set(asm_file "${CMAKE_CURRENT_BINARY_DIR}/${target}-${section}.s")
    get_filename_component(input_file "${file}" ABSOLUTE)
    file(SIZE "${input_file}" file_size)
    add_custom_command(
        OUTPUT "${asm_file}"
        COMMAND "${CMAKE_SOURCE_DIR}/Meta/generate-embedded-resource-assembly.sh" "${asm_file}" "${section}" "${input_file}" "${file_size}"
        DEPENDS "${input_file}" "${CMAKE_SOURCE_DIR}/Meta/generate-embedded-resource-assembly.sh"
        COMMENT "Generating ${asm_file}"
    )
    target_sources("${target}" PRIVATE "${asm_file}")
endfunction()
