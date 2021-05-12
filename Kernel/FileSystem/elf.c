#include <Kernel/FileSystem/elf.h>

uint32_t elf_load_executable(uint8_t* offset)
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

    if (header->instruction_set != OPSET_x86)
    {
        klog(ERROR, "Isn't the good ELF instruction set !");
        return FALSE;
    }

    /* Load the ELF file into memory. */
    uint32_t text_mem_offset = 0;

    ELF_program_header_t* program_headers = (ELF_program_header_t*) header->program_header;
    for (uint16_t i = 0; i < header->program_entry_header_numbers; i++)
    {
        ELF_program_header_t program_header = program_headers[i];
        printf("\tELF section %d: type: %d, size: %d\n", i, program_header.type, program_header.memory_size);

        switch(program_header.type)
        {
            case SEGMENT_NULL:
                break;
            case SEGMENT_LOAD:
                text_mem_offset = kmalloc(program_header.memory_size);
                memcpy(text_mem_offset, program_header.data_offset, program_header.memory_size);
                break;
        }
    }
        
    return text_mem_offset;
}