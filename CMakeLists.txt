cmake_minimum_required(VERSION 3.16)
project(TheOS C ASM)

include(${CMAKE_SOURCE_DIR}/Meta/CMake/utils.cmake)

set(THEOS_ARCH "i686" CACHE STRING "Target architecture for TheOS.")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS ON)

set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/Toolchain/Local/${THEOS_ARCH}/bin)
set(TOOLCHAIN_PREFIX ${TOOLCHAIN_PATH}/${THEOS_ARCH}-pc-theos-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msoft-float -O -fno-stack-protector -ffreestanding -nostdlib -nostdinc -static -I${CMAKE_SOURCE_DIR}/Includes -I${CMAKE_SOURCE_DIR}/Includes/LibC")

add_custom_target(run
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/run.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/run.sh
    USES_TERMINAL
)

add_custom_target(iso
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/iso.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/iso.sh
    USES_TERMINAL
)

add_custom_target(mkinitrd
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/Meta/mkinitrd.sh
    COMMAND ${CMAKE_SOURCE_DIR}/Meta/mkinitrd.sh
    USES_TERMINAL
)

add_custom_target(setup-run
    COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND ${CMAKE_MAKE_PROGRAM} mkinitrd
    COMMAND ${CMAKE_MAKE_PROGRAM} iso
    COMMAND ${CMAKE_MAKE_PROGRAM} run
    USES_TERMINAL
)

include_directories(Userland/Libraries)
include_directories(.)
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(Userland)
add_subdirectory(Kernel)