#!/bin/sh

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

mkdir output 2>&1 > /dev/null

for soc in ssc325;do

make distclean
make infinity6_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6

mv u-boot.xz.img.bin output/u-boot-${soc}.bin
mv BOOT.bin output/boot-${soc}.bin

done

for soc in ssc335 ssc337 ssc335de ssc337de;do

make distclean
make infinity6b0_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6b0

cp u-boot.xz.img.bin output/u-boot-${soc}.bin
mv BOOT.bin output/boot-${soc}.bin

done