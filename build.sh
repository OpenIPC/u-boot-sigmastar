#!/bin/sh

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

mkdir output 2>&1 > /dev/null

for soc in ssc325;do

make distclean
make infinity6_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6
mv BOOT.bin output/u-boot-${soc}-universal.bin

done

for soc in ssc335 ssc337 ssc335de ssc337de;do

make distclean
make infinity6b0_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6b0
mv BOOT.bin output/u-boot-${soc}-universal.bin

done

for soc in ssc30kq;do

make distclean
make infinity6e_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot.sh 6e0
mv BOOT.bin output/u-boot-${soc}-universal.bin

done

for soc in ssc338q;do

make distclean
make infinity6e_spinand_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

./make_boot_spinand.sh
mv BOOT.bin output/u-boot-${soc}-nand.bin

done

for soc in ssc338q;do

make distclean
make infinity6e_spinand_defconfig
sed -i "s/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=y/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=n/g" .config
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}

cp u-boot_spinand.xz.img.bin output/u-boot-${soc}-ramfs.bin

done
