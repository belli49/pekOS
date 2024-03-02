#!/bin/sh
set -e
. ./iso.sh

qemu-system-x86_64 -serial stdio -cdrom pekos.iso
