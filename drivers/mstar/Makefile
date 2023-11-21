#
# Copyright (c) [2019~2020] SigmaStar Technology.
#
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License version 2 for more details.
#

obj-y += uart/
obj-$(CONFIG_MS_EMMC) += emmc/
obj-$(CONFIG_MS_SDMMC) += sdmmc/
obj-$(CONFIG_MS_NAND) += unfd/
ifneq ($(CONFIG_MS_NOR_ONEBIN), y)
obj-$(CONFIG_MS_ISP_FLASH) += flash_isp/
endif
obj-$(CONFIG_MS_NOR_ONEBIN) += fsp_qspi/
obj-$(CONFIG_MS_AESDMA) += aesdma/
ifneq ($(CONFIG_MS_NAND_ONEBIN), y)
obj-$(CONFIG_MS_SPINAND) += spinand/
endif
obj-y += flash/
obj-$(CONFIG_MS_NAND_ONEBIN) += fsp_qspi/
obj-$(CONFIG_MS_BDMA) += bdma/
#obj-$(CONFIG_MS_USB) += usb/usbdcore.o usb/usbtty.o usb/ehci_usb.o
obj-$(CONFIG_MS_USB) += usb20/
obj-$(CONFIG_MS_ENABLE_USB_LAN_MODULE) += usb20/
#obj-$(CONFIG_ENABLE_FIRST_EHC) +=  usb/drvusbmain.o usb/drvhostlib.o
#obj-$(CONFIG_ENABLE_SECOND_EHC) += usb/drvusbmain2.o usb/drvhostlib2.o
#obj-$(CONFIG_ENABLE_THIRD_EHC)	 +=	usb/drvusbmain3.o usb/drvhostlib3.o

#obj-$(CONFIG_MS_ENABLE_USB_LAN_MODULE) += usb/usb_lan.o usb/ax88772a.o
obj-$(CONFIG_MS_CMD_EYE) += usb/cmd_eye.o

obj-$(CONFIG_MS_EMAC) += emac/
obj-$(CONFIG_SSTAR_GMAC) += gmac/
obj-$(CONFIG_MS_GPIO) += gpio/
ifneq ($(CONFIG_MS_NOR_ONEBIN), y)
ifneq ($(CONFIG_MS_NAND_ONEBIN), y)
obj-$(CONFIG_MS_PARTITION) += partition/
endif
endif
obj-$(CONFIG_MS_SRCFG) += isp/
obj-$(CONFIG_SYS_I2C_MSTAR) += i2c/
obj-$(CONFIG_HW_WATCHDOG) += wdt/
obj-$(CONFIG_SSTAR_DISP) += disp/
obj-$(CONFIG_SSTAR_PNL) += panel/
obj-$(CONFIG_SSTAR_HDMITX) += hdmitx/
obj-$(CONFIG_SSTAR_JPD) += jpd/
obj-$(CONFIG_SSTAR_RGN) += rgn/
obj-$(CONFIG_SSTAR_MSPI) += spi/
obj-$(CONFIG_SSTAR_INI_PARSER) += iniparser/
obj-$(CONFIG_SSTAR_PWM) += pwm/
obj-$(CONFIG_SSTAR_SAR) += sar/
obj-$(CONFIG_SSTAR_AIO) += aio/
obj-$(CONFIG_SSTAR_KEYPAD) += keypad/
