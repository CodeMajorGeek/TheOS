CC = gcc
CFLAGS = -ansi -c

AS = as
LD = ld

EMU = qemu-system-x86_64 -boot a -no-fd-bootchk

SOURCE = src

OBJS = boot.bin

all: clean boot.img

run: all
	$(EMU) -fda boot.img

boot.img::
	dd if=/dev/zero of=$@ bs=512 count=2880

boot.img:: $(OBJS)
	dd if=$< of=$@ bs=512 count=1 conv=notrunc

boot.bin:
	$(MAKE) -C $(SOURCE)/boot/ all;
	mv $(SOURCE)/boot/$@ $@

clean:
	$(MAKE) -C $(SOURCE)/boot/ clean;
	rm -rf *.img *.out *.bin

%.o: %.S
	$(AS) -o $@ $<