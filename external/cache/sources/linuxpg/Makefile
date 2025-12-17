#
# Makefile for the kernel character device drivers.
#

#
# This file contains the font map for the default (hardware) font
#

ifneq ($(KERNELRELEASE),)
	obj-m	 := pgdrv.o
#	EXTRA_CFLAGS += -DDEBUG
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD :=$(shell pwd)

.PHONY:all
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) CROSS_COMPILE=mipsel-linux- ARCH=mips modules

.PHONY:clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) CROSS_COMPILE=mipsel-linux- ARCH=mips clean

endif
