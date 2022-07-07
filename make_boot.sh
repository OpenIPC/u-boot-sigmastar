#!/bin/sh

IPL_DIR=ipl/${1:-6b0}

dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=1024  count=48 seek=0
dd if=MXP_SF.bin of=BOOT.bin bs=1024  count=4 seek=60
dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=1024  count=60 seek=64
dd if=/dev/zero of=BOOT.bin bs=1024  count=44 seek=124
dd if=u-boot.xz.img.bin of=BOOT.bin bs=1024  count=128 seek=128
