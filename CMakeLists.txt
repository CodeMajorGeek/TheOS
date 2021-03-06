cmake_minimum_required(VERSION 3.16)
project(TheOS C ASM)

include(${CMAKE_SOURCE_DIR}/Meta/CMake/utils.cmake)

set(CMAKE_INSTALL_MESSAGE NEVER)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THEOS_ARCH "i686" CACHE STRING "Target architecture for TheOS.")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS ON)

unset(CMAKE_SYSROOT)
set(CMAKE_STAGING_PREFIX ${CMAKE_BINARY_DIR}/Root)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Root)
set(CMAKE_INSTALL_DATAROOTDIR ${CMAKE_BINARY_DIR}/Root/Res)

set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/Toolchain/Local/${THEOS_ARCH}/bin)
set(TOOLCHAIN_PREFIX ${TOOLCHAIN_PATH}/${THEOS_ARCH}-pc-theos-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msoft-float -O -fno-stack-protector -ffreestanding -nostdlib -nostdinc -static")

add_custom_target(run
    DEPENDS iso
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/run.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/run.sh
    USES_TERMINAL
)

add_custom_target(iso
    DEPENDS mkinitrd
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/iso.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/iso.sh
    USES_TERMINAL
)

add_custom_target(image
    DEPENDS qemu-image
)
add_custom_target(qemu-image
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/build-qemu-image.sh
    COMMAND ${CMAKE_COMMAND} -E env "THEOS_ROOT=${CMAKE_SOURCE_DIR}" "THEOS_ARCH=${THEOS_ARCH}" ${CMAKE_SOURCE_DIR}/Meta/build-qemu-image.sh
    BYPRODUCTS ${CMAKE_BINARY_DIR}/_disk_image
    USES_TERMINAL
)

add_custom_target(mkinitrd
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/mkinitrd.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/mkinitrd.sh
    USES_TERMINAL
)

add_custom_target(setup-run
    COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND ${CMAKE_MAKE_PROGRAM} image
    COMMAND ${CMAKE_MAKE_PROGRAM} mkinitrd
    COMMAND ${CMAKE_MAKE_PROGRAM} iso
    COMMAND ${CMAKE_MAKE_PROGRAM} run
    USES_TERMINAL
)

add_custom_target(install-run
    COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND ${CMAKE_MAKE_PROGRAM} iso
    COMMAND ${CMAKE_MAKE_PROGRAM} run
    USES_TERMINAL
)

include_directories(.)
include_directories(${CMAKE_BINARY_DIR})

add_link_options(--sysroot ${CMAKE_BINARY_DIR}/Root)

include_directories(Includes)
include_directories(Includes/LibC)

add_subdirectory(Userland)
add_subdirectory(Kernel)
