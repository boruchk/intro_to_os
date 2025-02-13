# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := lab3.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
# Getting location of our kernel build
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
# run kernel build system to make module
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif