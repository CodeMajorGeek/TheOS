#include <Kernel/FileSystem/elf.h>

bool elf_load_executable(uint8_t* offset)
{
    ELF_header_t* header = (ELF_header_t*) offset;

    /* Do all checks to be sure this ELF file is compatible. */
    if (header->magic != MAGIC)
    {
        klog(ERROR, "Isn't an ELF file !");
        return FALSE;
    }
    
    if (header->architecture != ARCH_x86)
    {
        klog(ERROR, "Isn't an 32-bit ELF file !");
        return FALSE;
    }
    
    if (header->type != TYPE_EXECUTABLE)
    {
         klog(ERROR, "Isn't the good ELF type !");
        return FALSE;
    }

    if (header->instruction_set != OPSET_UKNOWN)
    {
        klog(ERROR, "Isn't the good ELF instruction set !");
        return FALSE;
    }

    /* Load the ELF file into memory. */

    ELF_program_header_t* program_header = (ELF_program_header_t*) header->program_header;

    return TRUE;
}