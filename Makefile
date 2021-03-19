CROSS_PATH = ./cross-compiler/theos-i686/bin/

CC = $(CROSS_PATH)i686-elf-gcc
CPPFLAGS = 
CFLAGS = -msoft-float -O -fno-stack-protector -ffreestanding

GAS = $(CROSS_PATH)i686-elf-as

LD = $(CROSS_PATH)i686-elf-ld

OBJCPY = $(CROSS_PATH)i686-elf-objcopy

GRUB = grub-mkrescue
GRUB_CHECK = grub-file --is-x86-multiboot

AR = $(CROSS_PATH)i686-elf-ar

DEFINES = -Iincludes/ -Iincludes/clib/

EMU = qemu-system-x86_64 -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0

KOBJS = boot/boot.o
KOBJS += kernel/entry.o kernel/io.o kernel/serial.o kernel/logger.o kernel/tty.o kernel/memory.o kernel/keyboard.o
KOBJS += kernel/gdt_wrapper.o kernel/gdt.o kernel/interrupt.o kernel/idt_wrapper.o kernel/idt.o kernel/isr.o kernel/tss_wrapper.o kernel/tss.o
KOBJS += kernel/syscall.o kernel/ordered_array.o kernel/vmem.o kernel/kmem.o kernel/frame.o kernel/process_wrapper.o kernel/page.o kernel/task.o
KOBJS += kernel/timer.o kernel/vfs.o kernel/initrd.o
OBJS = clib/string.o clib/stdlib.o clib/stdio.o clib/sys/syscall.o

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -D__DEBUG
endif

all: clean toolchain TheOS.iso

run: CPPFLAGS += -D__USE_QEMU
run: all
	$(EMU) -cdrom TheOS.iso

gdb: all
	$(EMU) -cdrom TheOS.iso -s -S

clean:
	$(MAKE) -C boot/ clean;
	$(MAKE) -C kernel/ clean;
	$(MAKE) -C clib/ clean;
	rm -rf *.bin *.o *.log *.iso TheCLib iso/


toolchain: clean-toolchain TheCLib clib/crt0.o
	cp TheCLib $(CROSS_PATH)../lib/
	cp clib/crt0.o $(CROSS_PATH)../lib/

clean-toolchain:
	rm -rf $(CROSS_PATH)../lib/*

TheOS.iso: boot/grub.cfg TheOS.bin mkinitrd/initrd.img
	mkdir -p iso/boot/grub
	cp $< iso/boot/grub/
	cp TheOS.bin iso/boot/
	cp mkinitrd/initrd.img iso/boot/
	$(GRUB) -o $@ iso

TheOS.bin: TheOS.o
	$(OBJCPY) -R .note -R .comment -S $< $@
	$(GRUB_CHECK) $@

TheOS.o: CPPFLAGS += -D__THEOS_KERNEL
TheOS.o: kernel.ld $(KOBJS) TheCLib
	$(LD) -T $< -o $@ $(KOBJS) TheCLib

TheCLib: $(OBJS)
	$(AR) rcs $@ $^

%.o: %.S
	$(GAS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $<