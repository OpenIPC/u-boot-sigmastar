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

obj-y += soc-ecc.o
soc-ecc-objs := $(CONFIG_SYS_CONFIG_NAME)/drv/src/drvFCIE.o \
                $(CONFIG_SYS_CONFIG_NAME)/hal/src/halFCIE.o \
                $(CONFIG_SYS_CONFIG_NAME)/spinand_soc_ecc.o \

ccflags-y += -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)/hal/inc \
             -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)/drv/inc \
             -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)/ \
             -I$(srctree)/$(src)/../nand \
             -I$(srctree)/$(src)/../os \
             -I$(srctree)/$(src)/../../fsp_qspi/drv/inc \
             -I$(srctree)/$(src)/../../fsp_qspi/hal/inc