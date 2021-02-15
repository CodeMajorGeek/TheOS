
OBJS = boot.o

all: boot.bin

clean:
	rm -rf *.o

boot.bin: $(OBJS)
	$(LD) --oformat binary -e init -o $@ $^