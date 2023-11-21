#!/bin/bash

while getopts "a:c:" opt; do
  case $opt in
    a)
      alkaid_dir=$OPTARG
      ;;
    c)
      chip=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

uboot_dir=$PWD


if [ "${chip}" = "i5" ]; then
  declare -x PATH="/tools/toolchain/arm-linux-gnueabihf-4.8.3-201404/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  make infinity5_defconfig
  make clean; make
  if [ -d $alkaid_dir/project/board/i5/boot/nor/uboot ]; then
    cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/i5/boot/nor/uboot
    cp $uboot_dir/u-boot.mz.img.bin $alkaid_dir/project/board/i5/boot/nor/uboot
  fi

  make infinity5_spinand_defconfig
  make clean; make
  if [ -d $alkaid_dir/project/board/i5/boot/spinand/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/i5/boot/spinand/uboot
  fi
fi

if [ "${chip}" = "i6" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  make infinity6_defconfig
  make clean; make -j4
  if [ -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi

  make infinity6_spinand_defconfig
  make clean; make -j4
  if [ -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  if [ -d $alkaid_dir/project/board/${chip}/boot/spinand-ramdisk/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand-ramdisk/uboot/u-boot_spinand-ramdisk.xz.img.bin
  fi
fi

if [ "${chip}" = "i6b0" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity6b0_nor_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity6b0_nand_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi
if [ "${chip}" = "i6e" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity6e_nor_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity6e_nand_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot

  make infinity6e_sdmmc_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot/UBOOT
fi
if [ "${chip}" = "i2m" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity2m_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity2m_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi
if [ "${chip}" = "m6" ]; then
  declare -x PATH="/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make mercury6_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot/016A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot/016A
  fi
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot/017A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot/017A
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/016A
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/017A

  make mercury6_qfn_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot/018A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot/018A
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/018A

  make mercury6_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot/016A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot/016A
  fi
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot/017A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot/017A
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/016A
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/017A

  make mercury6_qfn_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot/018A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot/018A
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/018A

  make mercury6_emmc_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot/016A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot/016A
  fi
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot/017A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot/017A
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot/016A
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot/017A

  make mercury6_emmc_qfn_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot/018A ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot/018A
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot/018A
fi
if [ "${chip}" = "i7" ]; then
  declare -x PATH="/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity7_emmc_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot
  fi
  cp $uboot_dir/u-boot_emmc.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot

  make infinity7_nor_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity7_nand_onebin_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi

if [ "${chip}" = "m6p" ]; then
  declare -x PATH="/tools/toolchain/gcc-11.1.0-20210608-sigmastar-glibc-x86_64_arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make mercury6p_defconfig
  make clean; make -j16
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make mercury6p_spinand_defconfig
  make clean; make -j16
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot

  make mercury6p_demo_defconfig
  make clean; make -j16
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/u-boot_demo.xz.img.bin

  make mercury6p_demo_spinand_defconfig
  make clean; make -j16
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/u-boot_demo_spinand.xz.img.bin
fi

if [ "${chip}" = "i6c" ]; then
  declare -x PATH="/tools/toolchain/gcc-11.1.0-20210608-sigmastar-glibc-x86_64_arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity6c_defconfig
  make clean; make -j16
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity6c_spinand_defconfig
  make clean; make -j16
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot

  #make infinity6c_demo_defconfig
  #make clean; make -j16
  #cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/u-boot_demo.xz.img.bin

  #make infinity6c_demo_spinand_defconfig
  #make clean; make -j16
  #cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/u-boot_demo_spinand.xz.img.bin
fi

