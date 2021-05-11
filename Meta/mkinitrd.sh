#!/bin/sh

rm -f ../../mkinitrd/initrd.img
cp Root/bin/TheApp ../../mkinitrd/
make -C ../../mkinitrd/ all

cp ../../mkinitrd/initrd.img .