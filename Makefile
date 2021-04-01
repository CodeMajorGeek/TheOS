CROSS_PATH = ./Toolchain/Local/i686/bin/

CC = $(CROSS_PATH)i686-pc-theos-gcc
CPPFLAGS = 
CFLAGS = -msoft-float -O -fno-stack-protector -ffreestanding -nostdlib -nostdinc

GAS = $(CROSS_PATH)i686-pc-theos-as

LD = $(CROSS_PATH)i686-pc-theos-ld

OBJCPY = $(CROSS_PATH)i686-pc-theos-objcopy

GRUB = grub-mkrescue
GRUB_CHECK = grub-file --is-x86-multiboot

AR = $(CROSS_PATH)i686-pc-theos-ar

DEFINES = -IIncludes/ -IIncludes/LibC/

EMU = qemu-system-x86_64 -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0

KOBJS = Kernel/Boot/boot.o
KOBJS += Kernel/entry.o Kernel/io.o Kernel/serial.o Kernel/logger.o Kernel/tty.o Kernel/memory.o Kernel/keyboard.o
KOBJS += Kernel/gdt_wrapper.o Kernel/gdt.o Kernel/interrupt.o Kernel/idt_wrapper.o Kernel/idt.o Kernel/isr.o Kernel/tss_wrapper.o Kernel/tss.o
KOBJS += Kernel/syscall.o Kernel/ordered_array.o Kernel/vmem.o Kernel/kmem.o Kernel/frame.o Kernel/process_wrapper.o Kernel/page.o Kernel/task.o
KOBJS += Kernel/timer.o Kernel/vfs.o Kernel/initrd.o
OBJS = Userland/Libraries/LibC/string.o Userland/Libraries/LibC/stdlib.o Userland/Libraries/LibC/stdio.o Userland/Libraries/LibC/sys/syscall.o

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -D__DEBUG
endif

all: clean TheOS.iso

run: CPPFLAGS += -D__USE_QEMU
run: all
	$(EMU) -cdrom TheOS.iso

gdb: all
	$(EMU) -cdrom TheOS.iso -s -S

clean:
	$(MAKE) -C Kernel/Boot/ clean;
	$(MAKE) -C Kernel/ clean;
	$(MAKE) -C Userland/Libraries/LibC clean;
	rm -rf *.bin *.o *.log *.iso TheCLib iso/


toolchain: clean-toolchain TheCLib
	cp TheCLib $(CROSS_PATH)../lib/

clean-toolchain:
	rm -rf $(CROSS_PATH)../lib/*

TheOS.iso: Kernel/Boot/grub.cfg TheOS.bin mkinitrd/initrd.img
	mkdir -p iso/boot/grub
	cp $< iso/boot/grub/
	cp TheOS.bin iso/boot/
	cp mkinitrd/initrd.img iso/boot/
	$(GRUB) -o $@ iso

TheOS.bin: TheOS.o
	$(OBJCPY) -R .note -R .comment -S $< $@
	$(GRUB_CHECK) $@

TheOS.o: CPPFLAGS += -D__THEOS_KERNEL
TheOS.o: Kernel/kernel.ld $(KOBJS) TheCLib
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $(KOBJS) TheCLib
	$(LD) -T $< -o $@ $(KOBJS) TheCLib

TheCLib: $(OBJS)
	$(AR) rcs $@ $^

%.o: %.S
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $<
