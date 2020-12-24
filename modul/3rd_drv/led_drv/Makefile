

ROOTFS_DIR = /home/linux/source/rootfs/

APP_NAME = led_test

CROSS_COMPILE = /home/linux/gcc-4.6.4/bin/arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc

ifeq ($(KERNELRELEASE), )

KERNEL_DIR = /home/linux/linux-3.14-fs4412
CUR_DIR = $(shell pwd)

all :
	make -C  $(KERNEL_DIR) M=$(CUR_DIR) modules
	$(CC) $(APP_NAME).c  -o $(APP_NAME)

clean :
	make -C  $(KERNEL_DIR) M=$(CUR_DIR) clean
	rm -rf $(APP_NAME)

install:
	cp -raf *.ko $(APP_NAME)   $(ROOTFS_DIR)/drv_module


else

obj-m += led_drv.o


endif
