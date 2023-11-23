#!/bin/sh
IPL=ipl/$1

dd if=/dev/zero bs=1k count=128 | tr '\000' '\377' > BOOT.bin

if [ "$1" = "infinity6c" ]; then
	dd if=$IPL/IPL.bin of=BOOT.bin bs=1k seek=0 conv=notrunc status=none
	dd if=$IPL/IPL_CUST.bin of=BOOT.bin bs=1k seek=60 conv=notrunc status=none
else
	dd if=$IPL/IPL.bin of=BOOT.bin bs=1k seek=0 conv=notrunc status=none
	dd if=$IPL/MXP_SF.bin of=BOOT.bin bs=1k seek=60 conv=notrunc status=none
	dd if=$IPL/IPL_CUST.bin of=BOOT.bin bs=1k seek=64 conv=notrunc status=none
fi

dd if=u-boot.xz.img.bin of=BOOT.bin bs=1k seek=128 conv=notrunc status=none
