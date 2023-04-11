#!/bin/sh

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

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

for soc in ssc30kq;do

make distclean
make infinity6e_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6e0

cp u-boot.xz.img.bin output/u-boot-${soc}.bin
mv BOOT.bin output/boot-${soc}.bin

done

for soc in ssc338q;do

make distclean
make infinity6e_spinand_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot_spinand.sh

cp u-boot_spinand.xz.img.bin output/u-boot-${soc}_spinand.bin
mv BOOT.bin output/boot-${soc}_spinand.bin

done
