#!/bin/sh

rm -f ../../mkinitrd/initrd.img
make -C ../../mkinitrd/ all

cp ../../mkinitrd/initrd.img .