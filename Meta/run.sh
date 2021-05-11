#!/bin/sh

[ -z "$THEOS_DISK_IMAGE" ] && THEOS_DISK_IMAGE="_disk_image"

[ -z "$THEOS_RAM_SIZE" ] && THEOS_RAM_SIZE=512M

[ -z "$THEOS_QEMU_CPU" ] && THEOS_QEMU_CPU="max"

qemu-system-x86_64 -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0 \
-s -m $THEOS_RAM_SIZE \
-cpu $THEOS_QEMU_CPU \
-d guest_errors \
-smp 2 \
-device VGA,vgamem_mb=64 \
-drive file=${THEOS_DISK_IMAGE},format=raw,index=0,media=disk -device ich9-ahci -usb -cdrom TheOS.iso