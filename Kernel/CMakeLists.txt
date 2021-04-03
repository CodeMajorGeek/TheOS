set(KERNEL_ARCH i386)

set(KERNEL_SOURCES
    entry.c
    Boot/boot.S
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
    kmem.c
    vmem.c
    frame.c
)

set(LIBC_SOURCES
    ../Userland/Libraries/LibC/stdlib.c
    ../Userland/Libraries/LibC/stdio.c
    ../Userland/Libraries/LibC/string.c
    ../Userland/Libraries/LibC/sys/syscall.c
)

set(SOURCES
    ${KERNEL_SOURCES}
    ${LIBC_SOURCES}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Boot/grub.cfg ${CMAKE_BINARY_DIR}/Kernel/Boot/grub.cfg COPYONLY)

add_link_options(LINKER:-T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld -nostdlib)

add_compile_definitions(__THEOS_KERNEL)
add_compile_definitions(__USE_QEMU)

add_executable(Kernel ${SOURCES})
set_target_properties(Kernel PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld)

theos_install_headers(Kernel)
theos_install_sources(Kernel)