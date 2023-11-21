#
# Makefile for AIO (Audio I/O) device driver.

CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))

# general options
ccflags-y             	 +=  -I$(srctree)/$(src)/../../../include/linux
ccflags-y             	 +=  -I$(srctree)/$(src)/../common

#----------------------- drv -----------------------
ccflags-y                +=  -I$(srctree)/$(src)/drv/inc
ccflags-y                +=  -I$(srctree)/$(src)/drv/pub

#----------------------- hal -----------------------
ccflags-y                +=  -I$(srctree)/$(src)/hal/common
ccflags-y                +=  -I$(srctree)/$(src)/hal/$(CONFIG_SYS_CONFIG_NAME)/pub
ccflags-y                +=  -I$(srctree)/$(src)/hal/$(CONFIG_SYS_CONFIG_NAME)/inc

#--------------------- sources ---------------------
obj-y +=./drv/src/common/mhal_audio.o
obj-y +=./drv/src/common/drv_audio.o
obj-y +=./drv/src/linux/drv_audio_api_linux.o
#obj-y +=./drv/src/linux/drv_audio_export.o
obj-y +=./hal/$(CONFIG_SYS_CONFIG_NAME)/src/hal_audio.o
#obj-y +=./hal/$(CONFIG_SYS_CONFIG_NAME)/src/hal_audio_os_api_linux.o
obj-y +=./hal/$(CONFIG_SYS_CONFIG_NAME)/src/hal_audio_sys.o
obj-y +=./hal/$(CONFIG_SYS_CONFIG_NAME)/src/hal_audio_reg.o
obj-y +=./hal/$(CONFIG_SYS_CONFIG_NAME)/src/hal_audio_config.o
obj-y +=./drv/src/common/audio_lineout.o