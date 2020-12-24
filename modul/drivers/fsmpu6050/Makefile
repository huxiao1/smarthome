export ARCH=arm
export CROSS_COMPILE=arm-linux-

#KERNELDIR :=/lib/modules/$(shell uname -r)/build
KERNELDIR :=/home/linux/linux-3.14-fs4412
PWD  :=$(shell pwd)

obj-m += fsmpu6050.o  

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gcc test.c -o mpu6050_test

install:
	cp  mpu6050_test *.ko ~/source/rootfs/drv_module/
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf mpu6050_test


