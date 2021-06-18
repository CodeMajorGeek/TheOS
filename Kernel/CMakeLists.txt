set(KERNEL_ARCH i386)

set(KERNEL_SOURCES
    entry.c
    Boot/boot.S
    CPU/gdt.c
    CPU/gdt_wrapper.S
    CPU/idt.c
    CPU/idt_wrapper.S
    FileSystem/initrd.c
    CPU/interrupt.S
    CPU/io.c
    CPU/isr.c
    Devices/keyboard.c
    Utils/logger.c
    Memory/memory.c
    Utils/ordered_array.c
    Memory/page.c
    Process/process_wrapper.S
    CPU/serial.c
    Process/syscall.c
    Process/task.c
    CPU/timer.c
    CPU/tss.c
    CPU/tss_wrapper.S
    Devices/tty.c
    FileSystem/vfs.c
    Memory/kmem.c
    Memory/vmem.c
    Memory/frame.c
    PCI/ide.c
)

set(LIBC_SOURCES
    ../Userland/Libraries/LibC/stdlib.c
    ../Userland/Libraries/LibC/stdio.c
    ../Userland/Libraries/LibC/string.c
    ../Userland/Libraries/LibC/sys/syscall.c
    ../Userland/Libraries/LibC/unistd.c
    ../Userland/Libraries/LibC/config/keyboard.c
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