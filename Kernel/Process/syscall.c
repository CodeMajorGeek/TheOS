#include <Kernel/Process/syscall.h>

void* SYSCALLS[MAX_SYSCALLS] = 
{
    &puts
};

void syscall_handler(registers_t* r)
{
    if (r->eax > MAX_SYSCALLS)
        return;

    void* location = SYSCALLS[r->eax];

    int ret;
    __asm__ __volatile__(" \
push %1; \
push %2; \
push %3; \
push %4; \
push %5; \
call *%6; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx;" : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (location));
    r->eax = ret;
}