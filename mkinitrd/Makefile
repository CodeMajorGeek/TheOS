CC = gcc
CFLAGS =

all: clean mkinitrd initrd
	
clean:
	rm -rf mkinitrd

mkinitrd: mkinitrd.c
	$(CC) $(CFLAGS) -o $@ $^
	chmod +x $@

initrd: mkinitrd
	./$< text.txt text.txt
