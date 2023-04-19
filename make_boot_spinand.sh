#!/bin/sh
IPL_DIR=ipl/${1:-6e_spinand}

dd if=/dev/zero bs=2k count=1920 | tr '\000' '\377' > BOOT.bin

dd if=${IPL_DIR}/CIS.bin of=BOOT.bin bs=2k seek=0 conv=notrunc status=none
dd if=${IPL_DIR}/CIS.bin of=BOOT.bin bs=2k seek=64 conv=notrunc status=none

dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=2k seek=640 conv=notrunc status=none
dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=2k seek=832 conv=notrunc status=none

dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=2k seek=1024 conv=notrunc status=none
dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=2k seek=1216 conv=notrunc status=none

dd if=u-boot_spinand.xz.img.bin of=BOOT.bin bs=2k seek=1408 conv=notrunc
dd if=u-boot_spinand.xz.img.bin of=BOOT.bin bs=2k seek=1600 conv=notrunc
