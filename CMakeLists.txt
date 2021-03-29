cmake_minimum_required(VERSION 3.16)
project(TheOS C CXX ASM)

include(${CMAKE_SOURCE_DIR}/Meta/CMake/utils.cmake)

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(FATAL
    ": Don't use CMAKE_BUILD_TYPE when building TheOS.\n"
    "The default build type is optimized with debug info and asserts enabled,\n"
    "and that's all there is.")
endif()

set(CMAKE_INSTALL_MESSAGE NEVER)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THEOS_ARCH "i686" CACHE STRING "Target architecture for TheOS.")

# Central location for all custom options used in the TheOS build.
option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer testing in gcc/clang" OFF)
option(ENABLE_KERNEL_ADDRESS_SANITIZER "Enable kernel address sanitizer testing in gcc/clang" OFF)
option(ENABLE_MEMORY_SANITIZER "Enable memory sanitizer testing in gcc/clang" OFF)
option(ENABLE_UNDEFINED_SANITIZER "Enable undefined behavior sanitizer testing in gcc/clang" OFF)
option(ENABLE_FUZZER_SANITIZER "Enable fuzzer sanitizer testing in clang" OFF)
option(ENABLE_ALL_THE_DEBUG_MACROS "Enable all debug macros to validate they still compile" OFF)
option(ENABLE_COMPILETIME_FORMAT_CHECK "Enable compiletime format string checks" ON)
option(ENABLE_PCI_IDS_DOWNLOAD "Enable download of the pci.ids database at build time" ON)
option(BUILD_LAGOM "Build parts of the system targeting the host OS for fuzzing/testing" OFF)

add_custom_target(run
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/run.sh
    USES_TERMINAL
)

# This can currently only be implemented by ordered commands
# as cmake doesn't support inter dependency ordering, and we
# would like to avoid inject dependencies on the existing
# custom commands to allow people to run commands adhoc with
# out forcing re-builds when they might not want them.
add_custom_target(setup-and-run
    COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND ${CMAKE_MAKE_PROGRAM} image
    COMMAND ${CMAKE_MAKE_PROGRAM} run
    USES_TERMINAL
)

add_custom_target(image
    DEPENDS qemu-image
)
add_custom_target(qemu-image
    COMMAND ${CMAKE_COMMAND} -E env "THEOS_ROOT=${CMAKE_SOURCE_DIR}" "THEOS_ARCH=${THEOS_ARCH}" ${CMAKE_SOURCE_DIR}/Meta/build-image-qemu.sh
    BYPRODUCTS ${CMAKE_BINARY_DIR}/_disk_image
    USES_TERMINAL
)
add_custom_target(grub-image
    COMMAND ${CMAKE_COMMAND} -E env "THEOS_ROOT=${CMAKE_SOURCE_DIR}" "THEOS_ARCH=${THEOS_ARCH}" ${CMAKE_SOURCE_DIR}/Meta/build-image-grub.sh
    BYPRODUCTS ${CMAKE_BINARY_DIR}/grub_disk_image
    USES_TERMINAL
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fconcepts -Wno-literal-suffix)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Wno-overloaded-virtual -Wno-user-defined-literals)
endif()

if (ENABLE_ALL_THE_DEBUG_MACROS)
    include(${CMAKE_SOURCE_DIR}/Meta/CMake/all_the_debug_macros.cmake)
endif(ENABLE_ALL_THE_DEBUG_MACROS)

include_directories(Userland/Libs)
include_directories(.)
include_directories(${CMAKE_BINARY_DIR})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

unset(CMAKE_SYSROOT)
set(CMAKE_STAGING_PREFIX ${CMAKE_BINARY_DIR}/Root)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Root)
set(CMAKE_INSTALL_DATAROOTDIR ${CMAKE_BINARY_DIR}/Root/res)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES TheOS)
    message("Good job on building cmake!")
else()
    set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/Toolchain/Local/${THEOS_ARCH}/bin)
    set(TOOLCHAIN_PREFIX ${TOOLCHAIN_PATH}/${THEOS_ARCH}-pc-theos-)

    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
    set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}ranlib)
    set(CMAKE_STRIP ${TOOLCHAIN_PREFIX}strip)
    set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)
endif()

foreach(lang ASM C CXX OBJC OBJCXX)
    unset(CMAKE_${lang}_OSX_COMPATIBILITY_VERSION_FLAG)
    unset(CMAKE_${lang}_OSX_CURRENT_VERSION_FLAG)
    unset(CMAKE_${lang}_LINK_FLAGS)
    unset(CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS)
    unset(CMAKE_SHARED_MODULE_CREATE_${lang}_FLAGS)
    unset(CMAKE_SHARED_MODULE_LOADER_${lang}_FLAG )
    unset(CMAKE_${lang}_OSX_DEPLOYMENT_TARGET_FLAG)
    unset(CMAKE_${lang}_SYSROOT_FLAG)
    if (CMAKE_SYSTEM_NAME MATCHES Darwin)
        ## MacOS Workaround. Don't generate install_name flag when cross compiling
        set(CMAKE_${lang}_CREATE_SHARED_LIBRARY
            "<CMAKE_${lang}_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS> <LINK_FLAGS> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
    endif()
endforeach()

set(CMAKE_INSTALL_NAME_TOOL "")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-shared -Wl,--hash-style=gnu,-z,relro,-z,now,-z,noexecstack")
set(CMAKE_CXX_LINK_FLAGS "-Wl,--hash-style=gnu,-z,relro,-z,now,-z,noexecstack")

# We disable it completely because it makes cmake very spammy.
# This will need to be revisited when the Loader supports RPATH/RUN_PATH.
set(CMAKE_SKIP_RPATH TRUE)

add_compile_options(-Wformat=2)
add_compile_options(-fdiagnostics-color=always)

if (NOT ${CMAKE_HOST_SYSTEM_NAME} MATCHES SerenityOS)
    # FIXME: Something makes this go crazy and flag unused variables that aren't flagged as such when building with the toolchain.
    #        Disable -Werror for now.
    add_compile_options(-Werror)
endif()

add_compile_options(-Wall)
add_compile_options(-Wextra)

add_compile_options(-Wno-address-of-packed-member)
add_compile_options(-Wcast-qual)
add_compile_options(-Wno-deprecated-copy)
add_compile_options(-Wno-expansion-to-defined)
add_compile_options(-Wimplicit-fallthrough)
add_compile_options(-Wmissing-declarations)
add_compile_options(-Wno-nonnull-compare)
add_compile_options(-Wno-unknown-warning-option)
add_compile_options(-Wundef)
add_compile_options(-Wwrite-strings)

add_compile_options(-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.)
add_compile_options(-fno-exceptions)
add_compile_options(-fstack-protector-strong)
add_compile_options(-g1)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wno-literal-suffix)

    add_compile_options(-fconcepts)
    add_compile_options(-fstack-clash-protection)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Wno-overloaded-virtual)
    add_compile_options(-Wno-user-defined-literals)
endif()

add_compile_definitions(DEBUG SANITIZE_PTRS)
set(CMAKE_CXX_FLAGS_STATIC "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie -fpic")

if (ENABLE_COMPILETIME_FORMAT_CHECK)
    add_compile_definitions(ENABLE_COMPILETIME_FORMAT_CHECK)
endif()

add_link_options(--sysroot ${CMAKE_BINARY_DIR}/Root)

include_directories(Userland/Libs/LibC)
include_directories(Userland/Libs/LibM)
include_directories(Userland)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Userland/Libs)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Userland)

add_subdirectory(Kernel)
add_subdirectory(Userland)