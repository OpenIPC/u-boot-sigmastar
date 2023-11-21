CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))

obj-$(CONFIG_SSTAR_PWM) += mdrv_pwm.o \
							$(CONFIG_SYS_CONFIG_NAME)/hal_pwm.o


ccflags-y += -I$(srctree)/$(src)/
ccflags-y += -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)/