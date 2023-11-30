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

ccflags-y += -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)

obj-y += dwc_eth_qos.o
obj-y += cmd_gmac.o
obj-y += mdrv_gmac.o
obj-y += $(CONFIG_SYS_CONFIG_NAME)/mhal_gmac.o
obj-y += icplus.o 
obj-y += albany.o
obj-y += motorcomm.o
