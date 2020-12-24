export ARCH=arm
export CROSS_COMPILE=arm-linux-

#KERNELDIR :=/lib/modules/$(shell uname -r)/build
KERNELDIR :=/home/fengjunhui/kernel/linux-3.14
PWD  :=$(shell pwd)

obj-m +=fsadc.o 

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gcc test.c -o adc_test

install:
	cp adc_test *.ko ~/source/rootfs/drv/
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf adc_test


