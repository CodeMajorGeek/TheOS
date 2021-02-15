CC = gcc -m32
CFLAGS = -msoft-float -O -fno-stack-protector -fno-exceptions -fno-builtin -fno-pie -g -ffreestanding
CPPFLAGS = -nostdinc

LD = ld -melf_i386

DEFINES = -I../includes/

OBJS = kernel_entry.o

all: kernel

clean:
	rm -rf *.o

kernel: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $< $(DEPS)