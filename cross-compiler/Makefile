TAR = tar

BINUTILS_SRC = binutils-2.36.tar.xz
GCC_SRC = gcc-10.2.0.tar.gz

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

TARGET = i686-elf
PREFIX = $(ROOT_DIR)/theos-i686

install: binutils/ gcc/

uninstall: clean
	rm -rf $(PREFIX)

clean:
	rm -rf binutils/ gcc/

binutils/: $(BINUTILS_SRC)
	wget https://ftp.gnu.org/gnu/binutils/$(BINUTILS_SRC)
	mkdir $@
	$(TAR) xf $< -C $@ --strip-components 1
	cd $@; \
	bash configure --target=$(TARGET) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror
	$(MAKE) -C $@
	$(MAKE) -C $@ install

gcc/: $(GCC_SRC)
	wget https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/$(GCC_SRC)
	mkdir -p $@build
	$(TAR) xf $< -C $@ --strip-components 1
	cd $@; \
	bash contrib/download_prerequisites
	cd $@build; \
	bash ../configure --target=$(TARGET) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers; \
	$(MAKE) -C $@ all-gcc; \
	$(MAKE) -C $@ all-target-libgcc; \
	$(MAKE) -C $@ install-gcc; \
	$(MAKE) -C $@ install-target-libgcc