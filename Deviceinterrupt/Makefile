obj-m := Deviceinterrupt.o

CROSS_COMPILE = arm-linux-gnueabihf-
ARCH = arm

KDIR	:= /home/choijunho/Desktop/V-Raptor_BMC_Won/kernel
PWD		:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH)  modules
