CC = gcc -m32
CPPFLAGS = -nostdinc -P
CFLAGS = -msoft-float -O -fno-stack-protector -fno-exceptions -fno-builtin -fno-pie -g -ffreestanding
CPPFLAGS = -nostdinc
ASFLAGS = -Wa,--gstabs

LD = ld -melf_i386

OBJCPY = objcopy

DEFINES = -Iincludes/

EMU = qemu-system-x86_64 -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0

OBJS = boot/setup.o boot/start.o boot/memory.o
OBJS += kernel/entry.o kernel/io.o kernel/serial.o kernel/logger.o

all: clean os.bin

run: all
	$(EMU) -fda os.bin

clean:
	$(MAKE) -C boot/ clean;
	$(MAKE) -C kernel/ clean;
	rm -rf *.out *.bin *.ld *.o *.log

os.bin: boot.bin kernel.bin
	cat $^ > $@

boot.bin: boot/boot.o
	$(LD) -N -e 0 -Ttext 0x7c00 --oformat binary -o $@ $^

kernel.ld: CPPFLAGS += -P
kernel.ld: kernel.ld.S
	$(CC) -E $(ASFLAGS) $(CPPFLAGS) $(DEFINES) -c $< -o $@

kernel.o: kernel.ld $(OBJS)
	$(LD) -T $< -o $@ $(OBJS)

kernel.bin: kernel.o
	$(OBJCPY) -R .note -R .comment -S $< $@

%.o: %.S
	$(CC) -o $@ -c $< $(ASFLAGS) $(CPPFLAGS) $(DEFINES)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $<