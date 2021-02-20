CC = gcc -m32
CPPFLAGS = -nostdinc -P
CFLAGS = -msoft-float -O -fno-stack-protector -fno-exceptions -fno-builtin -fno-pie -g -ffreestanding
CPPFLAGS = -nostdinc

GAS = as --32

LD = ld -m elf_i386

OBJCPY = objcopy

GRUB = grub-mkrescue
GRUB_CHECK = grub-file --is-x86-multiboot

DEFINES = -Iincludes/ -Iincludes/clib/

EMU = qemu-system-x86_64 -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0

KOBJS = boot/boot.o
KOBJS += kernel/entry.o kernel/io.o kernel/serial.o kernel/logger.o kernel/tty.o kernel/memory.o kernel/keyboard.o
KOBJS += kernel/gdt_wrapper.o kernel/gdt.o kernel/interrupt.o kernel/idt_wrapper.o kernel/idt.o kernel/isr.o kernel/tss_wrapper.o kernel/tss.o
KOBJS += kernel/syscall.o kernel/ordered_array.o kernel/vmem.o kernel/kmem.o kernel/frame.o kernel/process_wrapper.o kernel/page.o
OBJS = clib/string.o clib/stdlib.o clib/stdio.o

all: clean TheOS.iso

run: all
	$(EMU) -cdrom TheOS.iso

gdb: all
	$(EMU) -cdrom TheOS.iso -s -S

clean:
	$(MAKE) -C boot/ clean;
	$(MAKE) -C kernel/ clean;
	$(MAKE) -C clib/ clean;
	rm -rf *.bin *.o *.log *.iso iso/

TheOS.iso: boot/grub.cfg TheOS.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/grub/
	cp TheOS.bin iso/boot/
	$(GRUB) -o $@ iso

TheOS.bin: TheOS.o
	$(OBJCPY) -R .note -R .comment -S $< $@
	$(GRUB_CHECK) $@

TheOS.o: CPPFLAGS += -D__THEOS_KERNEL
TheOS.o: kernel.ld $(KOBJS) $(OBJS)
	$(LD) -T $< -o $@ $(KOBJS) $(OBJS)
	

%.o: %.S
	$(GAS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $<