add_compile_options(-Os)

if ("${THEOS_ARCH}" STREQUAL "i686")
    set(KERNEL_ARCH i386)
elseif("${THEOS_ARCH}" STREQUAL "x86_64")
    set(KERNEL_ARCH x86_64)
endif()

set(KERNEL_HEAP_SOURCES
    kmem.c
    vmem.c
    frame.c
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_STATIC}")

set(KERNEL_SOURCES
    entry.c
    gdt.c
    gdt_wrapper.S
    idt.c
    idt_wrapper.S
    initrd.c
    interrupt.S
    io.c
    isr.c
    keyboard.c
    logger.c
    memory.c
    ordered_array.c
    page.c
    process_wrapper.S
    serial.c
    syscall.c
    task.c
    timer.c
    tss.c
    tss_wrapper.S
    tty.c
    vfs.c
)

set(SOURCES
    ${KERNEL_SOURCES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option -Wvla")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie -fPIE -fno-rtti -ffreestanding -fbuiltin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-80387 -mno-mmx -mno-sse -mno-sse2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-asynchronous-unwind-tables")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
if (NOT ${CMAKE_HOST_SYSTEM_NAME} MATCHES TheoOS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib -nostdinc -nostdinc++")
endif()

if ("${THEOS_ARCH}" STREQUAL "x86_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large -mno-red-zone")
endif()

# Kernel Undefined Behavior Sanitizer (KUBSAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

# Kernel Address Sanitize (KASAN) implementation is still a work in progress, this option
# is not currently meant to be used, besides when developing Kernel ASAN support.
#
if (ENABLE_KERNEL_ADDRESS_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=kernel-address")
endif()

add_compile_definitions(KERNEL)

# HACK: This is a workaround for CLion to grok the kernel sources.
#       It's needed because CLion doesn't understand the way we switch compilers mid-build.
add_compile_definitions(__theos__)

add_link_options(LINKER:-T ${CMAKE_CURRENT_BINARY_DIR}/kernel.ld -nostdlib)

# HACK: This is to work around a bug in CMake dependency resolution, the
#       kernel won't re-link when boot.S changes without this.
set_source_files_properties(entry.c
    PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Boot/boot.S
)

add_library(boot OBJECT Boot/boot.S)

add_library(kernel_heap STATIC ${KERNEL_HEAP_SOURCES})
file(GENERATE OUTPUT kernel.ld INPUT kernel.ld)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES TheoOS)
    include_directories(/usr/local/include/c++/10.2.0/)
else()
    include_directories(../Toolchain/Local/${THEOS_ARCH}/${THEOS_ARCH}-pc-theos/include/c++/10.2.0/)
    include_directories(../Toolchain/Local/${THEOS_ARCH}/${THEOS_ARCH}-pc-theos/include/c++/10.2.0/${THEOS_ARCH}-pc-theos/)
endif()

add_executable(Kernel ${SOURCES})
target_link_libraries(Kernel kernel_heap gcc stdc++)
add_dependencies(Kernel boot kernel_heap)
install(TARGETS Kernel RUNTIME DESTINATION boot)

add_custom_command(
    TARGET Kernel
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mkmap.sh
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel.map DESTINATION res)

theos_install_headers(Kernel)
theos_install_sources(Kernel)