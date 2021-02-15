CC = gcc -m32
CPPFLAGS = -nostdinc -P
ASFLAGS = -Wa,--gstabs

LD = ld -melf_i386

OBJCPY = objcopy

DEFINES = -Iincludes/

EMU = qemu-system-x86_64

OBJS = boot/setup.o boot/start.o boot/memory.o
OBJS += kernel/kernel_entry.o

all: clean os.bin

run: all
	$(EMU) -fda os.bin

clean:
	$(MAKE) -C boot/ clean;
	$(MAKE) -C kernel/ clean;
	rm -rf *.out *.bin *.ld *.o

os.bin: boot.bin kernel.bin
	cat $^ > $@

boot.bin:
	$(MAKE) -C boot/ all;
	$(MAKE) -C kernel/ all;
	mv boot/$@ $@

kernel.ld: kernel.ld.S
	$(CC) -E $(ASFLAGS) $(CPPFLAGS) $(DEFINES) -c $< -o $@

kernel.o: kernel.ld $(OBJS)
	$(LD) -T $< -o $@ $(OBJS)

kernel.bin: kernel.o
	$(OBJCPY) -R .note -R .comment -S $< $@