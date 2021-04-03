#include <stdint.h>
#include <stdlib.h>

#if defined(__SSP__) || defined(__SSP_ALL__)
#error "SPP must be compiled without stack protection enabled on it. Have to use -fno-stack-protector."
#endif

extern uint32_t __stack_chk_guard;
uint32_t __stack_chk_guard = (uint32_t) 0xC6C7C8C9;

__attribute__((__noreturn__)) void __stack_chk_fail(void)
{
    // TODO: Do something on fail...
    abort();
}

__attribute__((__noreturn__)) void __stack_chk_fail_local(void)
{
    __stack_chk_fail();
}