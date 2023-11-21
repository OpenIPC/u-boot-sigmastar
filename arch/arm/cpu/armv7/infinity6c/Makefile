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

obj-y	:= chip.o
obj-$(CONFIG_MS_PIU_TIMER)	+= timer.o
#obj-y	+= ms_serial.o #moved to drivers/sstar/uart
obj-y   += board.o
obj-y   += reset.o
obj-$(CONFIG_SS_BOOTING_PERF)	+= prof.o

