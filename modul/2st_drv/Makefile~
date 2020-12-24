ROOTFS_DIR = /home/linux/source/rootfs
ifeq ($(KERNELRELEASE), )

KERNEL_DIR = /home/linux/linux-3.14-fs4412/
CUR_DIR = $(shell pwd)

all :
	make -C  $(KERNEL_DIR) M=$(CUR_DIR) modules

clean :
	make -C  $(KERNEL_DIR) M=$(CUR_DIR) clean
	
install:
	cp -raf *.ko   $(ROOTFS_DIR)/drv_module


else

obj-m += hello.o
obj-m += math.o

endif


