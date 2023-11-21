#!/bin/sh
IPL_DIR=ipl/${1:-i6e}

dd if=/dev/zero bs=1k count=2048 | tr '\000' '\377' > BOOT.bin

dd if=${IPL_DIR}/GCIS.bin of=BOOT.bin bs=1k seek=0 conv=notrunc status=none
dd if=${IPL_DIR}/PARTINFO.pni of=BOOT.bin bs=1k seek=2 conv=notrunc status=none
dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=1k seek=256 conv=notrunc status=none
dd if=${IPL_DIR}/IPL.bin of=BOOT.bin bs=1k seek=512 conv=notrunc status=none
dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=1k seek=768 conv=notrunc status=none
dd if=${IPL_DIR}/IPL_CUST.bin of=BOOT.bin bs=1k seek=1024 conv=notrunc status=none

dd if=u-boot_spinand.xz.img.bin of=BOOT.bin bs=1k seek=1280 conv=notrunc status=none
dd if=u-boot_spinand.xz.img.bin of=BOOT.bin bs=1k seek=1664 conv=notrunc status=none
