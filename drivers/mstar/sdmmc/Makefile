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

CHIP_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))
obj-y += src/hal_card_regs.o
obj-y += src/hal_card_timer.o

ifeq ($(CONFIG_ARCH_CEDRIC),y)
obj-y += src/hal_sdmmc.o
obj-y += src/hal_card_platform_c3.o
else ifeq ($(CONFIG_ARCH_CHICAGO),y)
obj-y += src/hal_sdmmc.o
obj-y += src/hal_card_platform_c4.o
else ifeq ($(CONFIG_ARCH_INFINITY),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF.o
else ifeq ($(CONFIG_ARCH_INFINITY3),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF3.o
else ifeq ($(CONFIG_ARCH_INFINITY5),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF5.o
else ifeq ($(CONFIG_ARCH_INFINITY6),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF6.o
else ifeq ($(CONFIG_ARCH_INFINITY2M),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF2m.o
EXTRA_CFLAGS    += -Idrivers/mstar/gpio/
EXTRA_CFLAGS    += -Idrivers/mstar/gpio/$(CHIP_NAME)
else ifeq ($(CONFIG_ARCH_INFINITY6E),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF6e.o
else ifeq ($(CONFIG_ARCH_INFINITY6B0),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF6b0.o
else ifeq ($(CONFIG_ARCH_PIONEER3),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_Pioneer3.o
EXTRA_CFLAGS    += -Idrivers/mstar/gpio/
EXTRA_CFLAGS    += -Idrivers/mstar/gpio/$(CHIP_NAME)
else ifeq ($(CONFIG_ARCH_MERCURY6),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_M6.o
else ifeq ($(CONFIG_ARCH_INFINITY7),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF7.o
else ifeq ($(CONFIG_ARCH_MERCURY6P),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_M6P.o
else ifeq ($(CONFIG_ARCH_INFINITY6C),y)
obj-y += src/hal_sdmmc_v5.o
obj-y += src/hal_card_platform_iNF6c.o
endif

obj-y += ms_sdmmc_ub.o
obj-y += ms_sdmmc_drv.o
obj-y += ms_sdmmc_verify.o
