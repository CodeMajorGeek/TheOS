#!/bin/sh

die() {
    echo "die: $*"
    exit 1
}

grub-file --is-x86-multiboot Kernel/Kernel || die()

rm -rf iso/
mkdir -p iso/boot/grub
cp Kernel/Boot/grub.cfg iso/boot/grub/
cp Kernel/Kernel iso/boot/TheOS
cp initrd.img iso/boot/
grub-mkrescue -o TheOS.iso iso/

exit 0
