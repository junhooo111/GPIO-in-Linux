obj-m := LEDdriver.o

KDIR    := /home/choijunho/Desktop/V-Raptor_BMC_Won/kernel
PWD    := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
