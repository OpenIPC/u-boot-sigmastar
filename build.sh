#!/bin/sh
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

rm -rf output
mkdir -p output

spinor() {
	make $2_defconfig
	make -j8 KCFLAGS=-DPRODUCT_SOC=$1
	sh make_boot_spinor.sh $2
	mv BOOT.bin output/u-boot-$1-nor.bin
	make distclean
}

spinand() {
	make $2_spinand_defconfig
	make -j8 KCFLAGS=-DPRODUCT_SOC=$1
	sh make_boot_spinand.sh $2
	mv BOOT.bin output/u-boot-$1-nand.bin
	make distclean
}

# spinor infinity6
for soc in ssc325; do
	spinor $soc infinity6
done

# spinand infinity6
for soc in ssc325de; do
	spinand $soc infinity6
done

# spinor infinity6b0
for soc in ssc333 ssc335 ssc337 ssc335de ssc337de; do
	spinor $soc infinity6b0
done

# spinand infinity6b0
for soc in ssc337de; do
	spinand $soc infinity6b0
done

# spinor infinity6c
for soc in ssc377; do
	spinor $soc infinity6c
done

# spinor infinity6e
for soc in ssc30kd ssc30kq ssc338q; do
	spinor $soc infinity6e
done

# spinand infinity6e
for soc in ssc338q; do
	spinand $soc infinity6e
done

# initramfs infinity6e
for soc in ssc338q; do
	make infinity6e_spinand_defconfig
	sed -i "s/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=y/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=n/g" .config
	make -j8 KCFLAGS=-DPRODUCT_SOC=$soc
	cp u-boot_spinand.xz.img.bin output/u-boot-$soc-ram.bin
	make distclean
done
