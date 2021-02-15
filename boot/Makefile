CC = gcc -m32
CPPFLAGS = -nostdinc
CFLAGS = -msoft-float -O -fno-stack-protector -fno-exceptions -fno-builtin -fno-pie -g -ffreestanding
ASFLAGS = -Wa,--gstabs

LD = ld -melf_i386

DEFINES = -I../includes/

KERNEL_BOOT_OBJS = setup.o memory.o start.o

all: boot.bin boot_kernel

clean:
	rm -rf *.o

boot.bin: boot.o
	$(LD) -N -e 0 -Ttext 0x7c00 --oformat binary -o $@ $^

boot_kernel: $(KERNEL_BOOT_OBJS)

%.o: %.S
	$(CC) -o $@ -c $< $(ASFLAGS) $(CPPFLAGS) $(DEFINES)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(DEFINES)