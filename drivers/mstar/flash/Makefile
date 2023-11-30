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

CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))
ccflags-$(CONFIG_MS_NAND_ONEBIN) += -I$(srctree)/$(src)/nand/ \
                                    -I$(srctree)/$(src)/fcie/$(CONFIG_SYS_CONFIG_NAME)/drv/inc \
                                    -I$(srctree)/$(src)/fcie/$(CONFIG_SYS_CONFIG_NAME)/
ccflags-$(CONFIG_MS_NOR_ONEBIN) += -I$(srctree)/$(src)/nor/
ccflags-y += -I$(srctree)/$(src)/
ccflags-y += -I$(srctree)/$(src)/os/
ccflags-y += -I$(srctree)/$(src)/../fsp_qspi/drv/inc
ccflags-y += -I$(srctree)/$(src)/../fsp_qspi/hal/inc
ccflags-y += -I$(srctree)/$(src)/../bdma/$(CONFIG_SYS_CONFIG_NAME)

obj-$(CONFIG_MS_NAND_ONEBIN) += nand/mdrvSpinandBbtBbm.o
obj-$(CONFIG_MS_NAND_ONEBIN) += nand/drvSPINAND.o
obj-$(CONFIG_MS_NAND_ONEBIN) += nand/mdrvPni.o
obj-$(CONFIG_MS_NAND_ONEBIN) += nand/spinand.o
obj-$(CONFIG_MS_NAND_ONEBIN) += os/mdrvFlashOsImpl.o
obj-$(CONFIG_MS_NAND_ONEBIN) += mdrvParts.o
obj-$(CONFIG_SS_SOC_ECC)     += fcie/

obj-$(CONFIG_MS_NOR_ONEBIN) += nor/drvSPINOR.o
obj-$(CONFIG_MS_NOR_ONEBIN) += nor/mdrvPni.o
obj-$(CONFIG_MS_NOR_ONEBIN) += nor/mtd_serflash.o
obj-$(CONFIG_MS_NOR_ONEBIN) += os/mdrvFlashOsImpl.o
obj-$(CONFIG_MS_NOR_ONEBIN) += mdrvParts.o
