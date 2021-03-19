CROSS_PATH = ../../cross-compiler/theos-i686/bin/

CC = $(CROSS_PATH)i686-elf-gcc
CFLAGS = -Wall $(CROSS_PATH)../lib/crt0.o -L$(CROSS_PATH)../lib/TheCLib
CPPFLAGS = -nostdlib -nostdinc -fno-builtin

DEFINES = -I../../includes/clib/

all: clean TheApp

clean:
	rm -rf TheCLib *.o

TheApp: TheCLib main.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ -c $^

TheCLib:
	$(MAKE) -C ../../ clean TheCLib;
	cp ../../TheCLib .

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFINES) -o $@ $<