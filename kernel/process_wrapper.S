.globl copy_page_physical
.globl read_eip
.globl perform_task_switch

copy_page_physical:
    push %ebx
    pushf
    cli
    mov 12(%esp), %ebx
    mov 16(%esp), %ecx

    mov %cr0, %edx
    and $0x7FFFFFFF, %edx
    mov %edx, %cr0

.loop:
    mov (%ebx), %eax
    mov %eax, (%ecx)
    add $4, %ebx
    add $4, %ecx
    dec %edx
    jnz .loop

    mov %cr0, %edx
    or $0x80000000, %edx
    mov %edx, %cr0

    popf
    pop %ebx
    ret

read_eip:
    pop %eax
    jmp %eax

perform_task_switch:
    cli
    mov 4(%esp), %ecx
    mov 8(%esp), %eax
    mov 12(%esp), %ebp
    mov 16(%esp), %esp
    mov %eax, %cr3
    mov $0x12345, %eax
    sti
    jmp *%ecx
