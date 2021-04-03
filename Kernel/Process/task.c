#include <Kernel/Process/task.h>

task_t* current_task;
task_t* ready_queue;

uint32_t initial_esp;

uint32_t next_pid = 1;

void task_init(uint32_t stack)
{
    initial_esp = stack;

    klog(INFO, "Enabling multitasking...");

    __asm__ __volatile__("cli"); // Disable interrupts.

    task_move_stack((void*) 0xE0000000, 0x2000); // Relocate the stack to know here it is.

    /* Initialize the first task (kernel one). */
    current_task = ready_queue = (task_t*) kmalloc(sizeof(task_t));
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = 0;
    current_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);

    __asm__ __volatile__("sti"); // Enable interrupts.
    klog(INFO, "Done.");
}

void task_switch(void)
{
    if (!current_task)
        return;

    uint32_t esp, ebp, eip;
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));
    eip = read_eip();

    if (eip == 0x12345)
        return;

    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;


    current_task = current_task->next;
    if (!current_task)
        current_task = ready_queue;

    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    current_directory = current_task->page_directory;

    tss_set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);
    perform_task_switch(eip, current_directory->phys_address, ebp, esp);
}

void task_move_stack(void* new_stack_start, uint32_t size)
{
    for (uint32_t i = (uint32_t) new_stack_start; i >= ((uint32_t) new_stack_start - size); i -= 0x1000)
        frame_alloc(page_get(i, true, current_directory), false, true);

    uint32_t pd_addr;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(pd_addr));
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(pd_addr));

    uint32_t old_stack_pointer;
    uint32_t old_base_pointer;
    __asm__ __volatile__("mov %%esp, %0" : "=r"(old_stack_pointer));
    __asm__ __volatile__("mov %%ebp, %0" : "=r"(old_base_pointer));
    uint32_t offset = (uint32_t) new_stack_start - initial_esp;
    uint32_t new_stack_pointer = old_stack_pointer + offset;
    uint32_t new_base_pointer = old_base_pointer + offset;

    memcpy((void*) new_stack_pointer, (void*) old_stack_pointer, initial_esp - old_stack_pointer);

    // FIXME: Insanity, have to do remaping.
    for (uint32_t i = (uint32_t) new_stack_start; i > (uint32_t) new_stack_start - size; i -= 4)
    {
        uint32_t tmp = *(uint32_t*) i;
        if ((old_stack_pointer < tmp) && (tmp < initial_esp))
        {
            tmp += offset;
            uint32_t* tmp2 = (uint32_t*) i;
            *tmp2 = tmp;
        }
    }
    __asm__ __volatile__("mov %0, %%esp" : : "r"(new_stack_pointer));
    __asm__ __volatile__("mov %0, %%ebp" : : "r"(new_base_pointer));
}

int task_fork(void)
{
    __asm__ __volatile__("cli");

    task_t* parent_task = (task_t*) current_task;
    page_directory_t* directory = (page_directory_t*) clone_directory(current_directory);

    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
    new_task->next = 0;

    task_t* tmp_task = (task_t*) ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;

    tmp_task->next = new_task;

    uint32_t eip = read_eip();

    if (current_task == parent_task)
    {
        uint32_t esp;
        __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
        uint32_t ebp;
        __asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;

        __asm__ __volatile__("sti");
        return new_task->id;
    }
    else
        return 0;
}

int task_getpid(void)
{
    return current_task->id;
}

void switch_to_user_mode(void)
{
    tss_set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);

    __asm__ __volatile__("\
                        cli;\
                        mov $0x23, %ax;\
                        mov %ax, %ds;\
                        mov %ax, %es;\
                        mov %ax, %fs;\
                        mov %ax, %gs;\
                        \
                        mov %esp, %eax;\
                        pushl $0x23;\
                        pushl %eax;\
                        pushf;\
                        \
                        pop %eax;\
                        or $0x200, %eax;\
                        push %eax;\
                        \
                        pushl $0x1B;\
                        push $1f;\
                        iret;\
                        1:");
}