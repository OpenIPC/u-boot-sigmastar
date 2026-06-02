#!/usr/bin/env bash

export ARCH=arm
export CROSS_COMPILE=arm-none-eabi-

rm -rf output
mkdir -p output

# dts_path=../output/ssc337/build/linux-custom/arch/arm/boot/dts
# board=infinity6b0-ssc009a-s01a

build() {
	soc=$1
	flash=$2

	case $soc in
		ssc325|ssc325de)
			family=infinity6
			;;
		ssc333|ssc335|ssc337|ssc335de|ssc337de)
			family=infinity6b0
			;;
		ssc377|ssc377d|ssc377de|ssc377qe|ssc378de|ssc378qe)
			family=infinity6c
			;;
		ssc30kd|ssc30kq|ssc338q)
			family=infinity6e
			;;
		*)
			echo "Unknown SOC: $soc"
			exit 1
			;;
	esac

	if [ $flash == "nor" ]; then
		board=$family
	else
		board=${family}_spi${flash}
	fi

	make ${board}_defconfig
	#cp -v .config configs/${board}_defconfig
	#return
	make -j8 KCFLAGS=-DPRODUCT_SOC=$soc # EXT_DTB=$dts_path/$board.dtb
	./create_img.sh
	sh make_boot_spi${flash}.sh ${family}
	mv BOOT.bin output/u-boot-$soc-${flash}.bin
	make distclean
}

# spinor infinity6
for soc in ssc325; do
	build $soc nor
done

# spinand infinity6
for soc in ssc325de; do
	build $soc nand
done

# spinor infinity6b0
for soc in ssc333 ssc335 ssc337 ssc335de ssc337de; do
	build $soc nor
done

# spinand infinity6b0
for soc in ssc337de; do
	build $soc nand
done

# spinor infinity6c
for soc in ssc377 ssc377d ssc377de ssc377qe ssc378de ssc378qe; do
	build $soc nor
done

# spinor infinity6e
for soc in ssc30kd ssc30kq ssc338q; do
	build $soc nor
done

# spinand infinity6e
for soc in ssc338q; do
	build $soc nand
done

exit 0
# initramfs infinity6e
for soc in ssc338q; do
	make infinity6e_spinand_defconfig
	sed -i "s/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=y/CONFIG_MS_SAVE_ENV_IN_NAND_FLASH=n/g" .config
	make -j8 KCFLAGS=-DPRODUCT_SOC=$soc
	cp u-boot_spinand.xz.img.bin output/u-boot-$soc-ram.bin
	make distclean
done
