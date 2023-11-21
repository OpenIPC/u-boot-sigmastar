#!/bin/sh
rm -Rf u-boot.bin.mz
rm -Rf u-boot.bin.xz

./mz c u-boot.bin u-boot.bin.mz
xz -z -k u-boot.bin u-boot.bin.xz
ms_ver="$(strings -a -T binary u-boot.bin | grep 'MVX' | grep 'UBT1501' | sed 's/\\*MVX/MVX/g' | cut -c 1-32)"
ld_addr=$(arm-linux-gnueabihf-gdb u-boot -ex 'p/x uboot_ld_addr' -ex 'quit' | grep '$1' | cut -d' ' -f3)
ep_addr=$(arm-linux-gnueabihf-gdb u-boot -ex 'p/x uboot_ep_addr' -ex 'quit' | grep '$1' | cut -d' ' -f3)

#out_file=u-boot.img.bin
out_file_mz=u-boot$1.mz.img.bin
out_file_xz=u-boot$1.xz.img.bin
out_file=u-boot$1.img.bin
if [ `echo $ms_ver | grep -c "MVX1S" ` -gt 0 ];then
  out_file_mz=u-boot_S.mz.img.bin
  out_file_xz=u-boot_S.xz.img.bin
fi

echo ""
echo $out_file_mz
#echo ./mkimage -A arm -O u-boot -C mz -a "$ld_addr" -e "$ep_addr" -n "$(echo $ms_ver)" -d u-boot.bin.mz "$out_file_mz"
./mkimage -A arm -O u-boot -C mz -a "$ld_addr" -e "$ep_addr" -n "$ms_ver" -d u-boot.bin.mz "$out_file_mz"
rm -Rf u-boot.bin.mz
echo ""

echo ""
echo $out_file_xz
#echo ./mkimage -A arm -O u-boot -C xz -a "$ld_addr" -e "$ep_addr" -n "$(echo $ms_ver)" -d u-boot.bin.xz "$out_file_xz"
./mkimage -A arm -O u-boot -C lzma -a "$ld_addr" -e "$ep_addr" -n "$ms_ver" -d u-boot.bin.xz "$out_file_xz"
rm -Rf u-boot.bin.xz
echo ""

echo ""
echo $out_file
#echo ./mkimage -A arm -O u-boot -C none -a "$ld_addr" -e "$ep_addr" -n "$(echo $ms_ver)" -d u-boot.bin "$out_file"
./mkimage -A arm -O u-boot -C none -a "$ld_addr" -e "$ep_addr" -n "$ms_ver" -d u-boot.bin "$out_file"
echo ""
