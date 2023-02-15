#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/pekos.kernel isodir/boot/pekos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "pekos" {
	multiboot /boot/pekos.kernel
}
EOF
grub-mkrescue -o pekos.iso isodir
