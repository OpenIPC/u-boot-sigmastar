
KDIR=${PWD}/../../../../../../..

EXTRA_CFLAGS        +=  -Idrivers/sstar/include
EXTRA_CFLAGS        +=  -Idrivers/sstar/camdriver/aio/drv/pub
EXTRA_CFLAGS        +=  -Idrivers/sstar/camdriver/common
EXTRA_CFLAGS        +=  -Idrivers/sstar/camdriver/aio/ut/linux/ko/include
EXTRA_CFLAGS        +=  -Idrivers/sstar/camdriver/aio/ut/linux/ko_include

obj-m += audio_ut_module.o

all:
	make ARCH=arm CROSS_COMPILE?=arm-linux-gnueabihf- -C $(KDIR) SUBDIRS=$(shell pwd) modules
	
clean:
	rm -rf *.mod.c *.o *.ko Module* modules*