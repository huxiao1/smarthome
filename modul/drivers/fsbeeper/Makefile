export ARCH=arm
export CROSS_COMPILE=arm-linux-

#KERNELDIR :=/lib/modules/$(shell uname -r)/build
KERNELDIR :=/home/fengjunhui/kernel/linux-3.14
PWD  :=$(shell pwd)

obj-m +=fspwmbeeper.o 

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gcc test.c -o music_test

install:
	sudo cp music_test *.ko ~/source/rootfs/drv/
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf music_test


