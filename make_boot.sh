#!/bin/sh
IPL_DIR=ipl/${1:-6b0}

dd if=/dev/zero bs=1k count=128 | tr '\000' '\377' > BOOT.bin

dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=1k seek=0 conv=notrunc status=none
dd if=${IPL_DIR}/MXP_SF.bin of=BOOT.bin bs=1k seek=60 conv=notrunc status=none
dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=1k seek=64 conv=notrunc status=none
dd if=u-boot.xz.img.bin of=BOOT.bin bs=1k seek=128 conv=notrunc status=none
