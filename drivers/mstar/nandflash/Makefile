CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))
obj-y += spi_nand.o
spi_nand-objs := src/drvSPINAND.o \
                 src/spinand.o \
                 src/spinand_bbt.o \
                 src/drvSPINANDOsImpl.o \
                 src/mdrvParts.o

ccflags-y += -I$(srctree)/$(src)/inc
ccflags-y += -I$(srctree)/$(src)/../fsp_qspi/drv/inc
ccflags-y += -I$(srctree)/$(src)/../fsp_qspi/hal/inc
ccflags-y += -I$(srctree)/$(src)/../bdma/$(CONFIG_SYS_CONFIG_NAME)
ccflags-y += -I$(srctree)/$(src)/../flash_util/inc
