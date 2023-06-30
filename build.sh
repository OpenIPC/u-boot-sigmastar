#!/bin/sh
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

rm -rf output
mkdir -p output

# spinor infinity6
for soc in ssc325; do
test -n "$1" && test "$soc" != "$1" && continue
make distclean
make infinity6_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}
./make_boot_spinor.sh i6
mv BOOT.bin output/u-boot-${soc}-universal.bin
done

# spinor infinity6b0
for soc in ssc335 ssc337 ssc335de ssc337de; do
test -n "$1" && test "$soc" != "$1" && continue
make distclean
make infinity6b0_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}
./make_boot_spinor.sh i6b0
mv BOOT.bin output/u-boot-${soc}-universal.bin
done

# spinor infinity6e
for soc in ssc30kq ssc338q; do
test -n "$1" && test "$soc" != "$1" && continue
make distclean
make infinity6e_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}
./make_boot_spinor.sh i6e
mv BOOT.bin output/u-boot-${soc}-universal.bin
done

# spinand infinity6e
for soc in ssc338q; do
test -n "$1" && test "$soc" != "$1" && continue
make distclean
make infinity6e_spinand_defconfig
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}
./make_boot_spinand.sh
mv BOOT.bin output/u-boot-${soc}-spinand.bin
done

# initramfs infinity6e
for soc in ssc338q; do
test -n "$1" && test "$soc" != "$1" && continue
make distclean
make infinity6e_spinand_defconfig
sed -i "s/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=y/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=n/g" .config
make -j5 KCFLAGS=-DPRODUCT_NAME=${soc}
cp u-boot_spinand.xz.img.bin output/u-boot-${soc}-initramfs.bin
done

make distclean
