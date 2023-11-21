CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))

obj-y += fsp_qspi.o
fsp_qspi-objs := drv/src/drvFSP_QSPI.o \
                 hal/src/halFSP_QSPI.o


ccflags-y += -I$(srctree)/$(src)/hal/inc \
             -I$(srctree)/$(src)/drv/inc \
             -I$(srctree)/arch/arm/include/asm/arch-$(CONFIG_SYS_CONFIG_NAME)/mach