#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

extra-y	:= start.o

obj-y	+= cache_v7.o cache_v7_asm.o

obj-y	+= cpu.o
obj-y	+= syslib.o

ifneq ($(CONFIG_AM43XX)$(CONFIG_AM33XX)$(CONFIG_OMAP44XX)$(CONFIG_OMAP54XX)$(CONFIG_TEGRA)$(CONFIG_MX6)$(CONFIG_TI81XX)$(CONFIG_AT91FAMILY)$(CONFIG_SUNXI),)
ifneq ($(CONFIG_SKIP_LOWLEVEL_INIT),y)
obj-y	+= lowlevel_init.o
endif
endif

ifneq ($(CONFIG_ARMV7_NONSEC)$(CONFIG_ARMV7_VIRT),)
obj-y	+= nonsec_virt.o
obj-y	+= virt-v7.o
obj-y	+= virt-dt.o
endif

ifneq ($(CONFIG_ARMV7_PSCI),)
obj-y	+= psci.o
endif

obj-$(CONFIG_IPROC) += iproc-common/
obj-$(CONFIG_KONA) += kona-common/
obj-$(CONFIG_OMAP_COMMON) += omap-common/
obj-$(CONFIG_SYS_ARCH_TIMER) += arch_timer.o
obj-$(CONFIG_TEGRA) += tegra-common/

ifneq (,$(filter s5pc1xx exynos,$(SOC)))
obj-y += s5p-common/
endif

obj-$(if $(filter am33xx,$(SOC)),y) += am33xx/
obj-$(if $(filter armada-xp,$(SOC)),y) += armada-xp/
obj-$(CONFIG_AT91FAMILY) += at91/
obj-$(if $(filter bcm281xx,$(SOC)),y) += bcm281xx/
obj-$(if $(filter bcmcygnus,$(SOC)),y) += bcmcygnus/
obj-$(if $(filter bcmnsp,$(SOC)),y) += bcmnsp/
obj-$(CONFIG_ARCH_EXYNOS) += exynos/
obj-$(CONFIG_ARCH_HIGHBANK) += highbank/
obj-$(CONFIG_ARCH_KEYSTONE) += keystone/
obj-$(if $(filter ls102xa,$(SOC)),y) += ls102xa/
obj-$(if $(filter mx5,$(SOC)),y) += mx5/
obj-$(CONFIG_MX6) += mx6/
obj-$(CONFIG_OMAP34XX) += omap3/
obj-$(CONFIG_OMAP44XX) += omap4/
obj-$(CONFIG_OMAP54XX) += omap5/
obj-$(CONFIG_RMOBILE) += rmobile/
obj-$(CONFIG_ARCH_S5PC1XX) += s5pc1xx/
obj-$(CONFIG_SOCFPGA) += socfpga/
obj-$(if $(filter stv0991,$(SOC)),y) += stv0991/
obj-$(CONFIG_ARCH_SUNXI) += sunxi/
obj-$(CONFIG_TEGRA20) += tegra20/
obj-$(CONFIG_U8500) += u8500/
obj-$(CONFIG_ARCH_UNIPHIER) += uniphier/
obj-$(CONFIG_VF610) += vf610/
obj-$(CONFIG_ZYNQ) += zynq/
obj-$(CONFIG_CEDRIC) += cedric/
obj-$(CONFIG_CHICAGO) += chicago/
obj-$(CONFIG_INFINITY) += infinity/
obj-$(CONFIG_INFINITY3) += infinity3/
obj-$(CONFIG_INFINITY5) += infinity5/
obj-$(CONFIG_INFINITY6) += infinity6/
obj-$(CONFIG_INFINITY2M) += infinity2m/
obj-$(CONFIG_INFINITY6E) += infinity6e/
obj-$(CONFIG_INFINITY6B0) += infinity6b0/
obj-$(CONFIG_PIONEER3) += pioneer3/
obj-$(CONFIG_MERCURY6) += mercury6/
obj-$(CONFIG_INFINITY7) += infinity7/
obj-$(CONFIG_MERCURY6P) += mercury6p/
obj-$(CONFIG_INFINITY6C) += infinity6c/
