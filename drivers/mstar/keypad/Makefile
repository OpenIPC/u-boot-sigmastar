CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))

obj-$(CONFIG_SSTAR_KEYPAD) += mdrv_keypad.o \
                           $(CONFIG_SYS_CONFIG_NAME)/mhal_keypad.o

ccflags-y += -I$(srctree)/$(src)/../gpio/$(CONFIG_SYS_CONFIG_NAME)
ccflags-y += -I$(srctree)/$(src)/../gpio/
ccflags-y += -I$(srctree)/$(src)/
ccflags-y += -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)/
