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


ifdef CONFIG_JPD_SW
ccflags-y += -I$(srctree)/$(src)/jpd_sw/inc
ccflags-y += -DDJPD_OS_TYPE_UBOOT

obj-y += ./jpd_sw/src/jdapimin.o
obj-y += ./jpd_sw/src/jdapistd.o
obj-y += ./jpd_sw/src/jcomapi.o
obj-y += ./jpd_sw/src/jdtrans.o
obj-y += ./jpd_sw/src/jdmaster.o
obj-y += ./jpd_sw/src/jdinput.o
obj-y += ./jpd_sw/src/jdmainct.o
obj-y += ./jpd_sw/src/jdcoefct.o
obj-y += ./jpd_sw/src/jdpostct.o
obj-y += ./jpd_sw/src/jdmarker.o
obj-y += ./jpd_sw/src/jdhuff.o
obj-y += ./jpd_sw/src/jdarith.o
obj-y += ./jpd_sw/src/jddctmgr.o
obj-y += ./jpd_sw/src/jidctint.o
obj-y += ./jpd_sw/src/jidctfst.o
obj-y += ./jpd_sw/src/jidctflt.o
obj-y += ./jpd_sw/src/jdsample.o
obj-y += ./jpd_sw/src/jdcolor.o
obj-y += ./jpd_sw/src/jdmerge.o
obj-y += ./jpd_sw/src/jquant1.o
obj-y += ./jpd_sw/src/jquant2.o
obj-y += ./jpd_sw/src/jdatasrc.o
obj-y += ./jpd_sw/src/jaricom.o
obj-y += ./jpd_sw/src/jerror.o
obj-y += ./jpd_sw/src/jmemmgr.o
obj-y += ./jpd_sw/src/jutils.o
obj-y += ./jpd_sw/src/jmemmgr.o
obj-y += ./jpd_sw/src/jmemnobs.o
endif

ifdef CONFIG_JPD_HW
ccflags-y += -I$(srctree)/$(src)/../common
ccflags-y += -I$(srctree)/$(src)/../../../../boot/include
ccflags-y += -I$(srctree)/$(src)/../../../../boot/include/linux
ccflags-y += -I$(srctree)/$(src)/jpd_hw/drv/pub
ccflags-y += -I$(srctree)/$(src)/jpd_hw/drv/src
ccflags-y += -I$(srctree)/$(src)/jpd_hw/drv/inc
ccflags-y += -I$(srctree)/$(src)/jpd_hw/hal/common/inc
ccflags-y += -I$(srctree)/$(src)/jpd_hw/hal/common/src
ccflags-y += -I$(srctree)/$(src)/jpd_hw/hal/$(CONFIG_SYS_CONFIG_NAME)/inc

ifeq ($(CONFIG_SYS_CONFIG_NAME), mercury6p)
EXTRA_CFLAGS += -DCHIP_M6P
else ifeq ($(CONFIG_SYS_CONFIG_NAME), infinity7)
EXTRA_CFLAGS += -DCHIP_I7
endif

EXTRA_CFLAGS += -DJPD_SUPPORT
EXTRA_CFLAGS += -DUSE_PHYSICAL_ADDR

obj-y += jpd_hw/drv/src/mhal_jpd.o
obj-y += jpd_hw/drv/src/drv_jpd_dev.o
obj-y += jpd_hw/drv/src/drv_jpd_ctx.o
obj-y += jpd_hw/drv/src/drv_jpd_parser.o
obj-y += jpd_hw/drv/src/drv_jpd_vdi.o
obj-y += jpd_hw/hal/common/src/hal_jpd_ios.o
obj-y += jpd_hw/hal/common/src/hal_jpd_ops.o

endif


