CC = gcc
CFLAGS =

all: clean mkinitrd
	
clean:
	rm -rf mkinitrd

mkinitrd: mkinitrd.c
	$(CC) $(CFLAGS) -o $@ $^