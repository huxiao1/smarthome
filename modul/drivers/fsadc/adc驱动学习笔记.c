
调试技巧：

//可选择性的打印调试信息

int flags = 1;
//#define flags 1 

#define DEBUG_PRINTK(msg,debug_flags) \
    do{ \
        if(!!debug_flags) { \
            printk("---->%s--->%d\n",__func__,__LINE__);\
            printk(msg);\
		}\
    }while(0)

当flags为0的时候，不会出现调试的打印信息  //注意加{ }
		
static int fs4412_adc_open (struct inode *inod, struct file *filep)  
{  
  	DEBUG_PRINTK("fs4412_adc_open",flags);
    return 0;  
}  
		
			
 //错误处理函数： 
#define handle_error(msg) do{\
						perror(msg);\
				        printf("%s --> %s -->%d\n", __FILE__, __func__, __LINE__);\
						exit(EXIT_FAILURE);\
                    }while(0)
		

ADC 
	DEV原理图：
	
	电压最高1.8V，最低0V
	采样管脚	XadcAIN3 

	对应Core板： 
		XadcAIN3	
	
	查找芯片手册： 
		56章  --- ADC 
	
	查看概述信息：
	Overview of ADC
		The 10-bit or 12-bit CMOS Analog to Digital Converter (ADC) comprises of 4-channel analog inputs. It convertsthe analog input signal into 10-bit or 12-bit binary digital codes at a maximum conversion rate of 1MSPS with
		5MHz A/D converter clock. A/D converter operates with on-chip sample-and-hold function. ADC supports lowpower mode.
		
		10-12位 CMOS模数转换 
		0-3 共四路模拟采样通道
		将模拟信号转换为10-12位二进制代码 最高5Mhz转化时钟时的转化速率为1MSPS，
		支持低功耗模式
		
	查看寄存器信息：
		56.4.2 A/D Conversion Time
		When the APB bus clock(PCLK) frequency is 66MHz and the prescaler value is 65, total 12-bit conversion time is
	as follows.
	  A/D converter freq. = 66MHz/(65+1) = 1MHz
	  Conversion time = 1/(1MHz / 5cycles) = 1/200kHz = 5us
	NOTE:  This A/D converter was designed to operate at maximum 5MHz clock, so the conversion rate can go up to 1MSPS.	

	了解寄存器时做什么的
	#define  ADCCON  0x0000   ADC Control Register  0x0000_3FC4
	#define  ADCDLY  0x0008   ADC Start or Interval Delay Register  0x0000_00FF
	#define  ADCDAT  0x000C   ADC Conversion Data Register  Undefined
	#define  CLRINTADC 0x0018  Clear ADC Interrupt  Undefined
	#define  ADCMUX    0x001C  Specifies the Analog input channel selection  0x0000_0000	

	寄存器配置：
		ADCCON
		Base Address: 0x126C_0000
		
		ADCCON |= (1 << 0) | (1 << 16) | (1<<14)  | (99 <<6) ;
		
		 ADCDAT      采样的的数据值所在的寄存器
		Base Address: 0x126C + 0x000C
		
		ADCMUX (ADC Channel Mux Register)
  	Base Address: 0x126C_0000 + 0x001C, Reset Value = 0x0000_0000
		
		ADCMUX &~=(0xF) | (0X11);
		

	设备树解析：
		adc3_node@fs4412 {
			compatible = "fs4412,adc";
			reg = <0x126C0000 0x20>; 
			interrupt-parent = <&combiner>;
			interrupts = <10 3>;
		};
		
		对于ADC的中断，其位于combiner中
		
		Combiner Group ID  Combined Interrupt    Bit  Interrupt Source  Source Block
								Source Name

		INTG10   		DMC1/DMC0/MIU/L2CACHE	 [3]  	ADC  			General ADC
		
		SPI Port No  ID  Int_I_Combiner  Interrupt Source  Source Block
		10  	42		 IntG10_3   	 ADC 				 General ADC
		
		
		在使用平台设备的时候一般有这样的操作：
		先获取资源： 
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0); 
		然后通过获取的资源找出映射的虚拟寄存器地址 
		regs_addr = devm_ioremap_resource(&pdev->dev, res);

		但是为什么就可以通过res = platform_get_resource(pdev, IORESOURCE_MEM, 0);拿到资源？ 
		肯定有地方对regs_addr做了定义，并且和这个设备的platform_device进行了绑定 
		例如：(对应着真实硬件的信息)
		static struct resource s3c_i2c_resource[] = {
			 [0] = {
			  .start = S3C24XX_PA_IIC,
			  .end   = S3C24XX_PA_IIC + S3C24XX_SZ_IIC - 1,
			  .flags = IORESOURCE_MEM,
			 },
			  [1] = {
			  .start = IRQ_IIC,
			  .end   = IRQ_IIC,
			  .flags = IORESOURCE_IRQ,
			 }
		};
			这里就对一个platform_device设备进行了描述。
		然后注册该设备，当设备和driver匹配上后就可以得到该值。
		
		当然了也可以采用设备树的方式来实现： 
		
		设备树信息： 
		adc3_node@fs4412 {
			compatible = "fs4412,adc";
			reg = <0x126C0000 0x20>; 
			interrupt-parent = <&combiner>;
			interrupts = <10 3>;
		};
		
		static int fs4412_adc_probe(struct platform_device *pdev)  
		{  
			int ret;  
			printk("match 0k \n");  
			res1 = platform_get_resource(pdev,IORESOURCE_IRQ, 0);  
			res2 = platform_get_resource(pdev,IORESOURCE_MEM, 0);   
				 
			ret = request_irq(res1->start,fs4412_adc_handler,IRQF_DISABLED,"adc1",NULL);  
			fs4412_adc_base = ioremap(res2->start,res2->end-res2->start);  
		    printk("res1->start :%#x. res2->start :%#x.\n",res1->start,res2->start);

			register_chrdev(major, "adc", &fs4412_adc_ops);  
			init_waitqueue_head(&wq);  
			  
			return 0;  
		}  	
		
		//首先platform总线将驱动和设备树进行匹配，然后probe函数执行，然后通过
		//platform_get_resource来获得设备树中的资源。
		static struct of_device_id fs4412_adc_id[]=  
		{  
			{.compatible = "fs4412,adc" },  
		};  
		  
		static struct platform_driver fs4412_adc_driver=  
		{  
			.probe = fs4412_adc_probe,  
			.remove = fs4412_adc_remove,  
			.driver ={  
				.name = "bigbang",  
				.of_match_table = fs4412_adc_id,  
			},  
		};  
		  
		static int fs4412_adc_init(void)  
		{  
			return platform_driver_register(&fs4412_adc_driver);  
		}  
		static void fs4412_adc_exit(void)  
		{  
			platform_driver_unregister(&fs4412_adc_driver);  
		}  
		
		
		
		
		
		
		
		
			
		
		
		
3、设备驱动的大体流程：
read()
{
       1、向adc设备发送要读取的命令
          ADCCON    1<<0 | 1<<14 | 0X1<<16 | 0XFF<<6

       2、读取不到数据就休眠
            wait_event_interruptible();

       3、等待被唤醒读数据
          havedata = 0；
}

adc_handler（）
{
       1、清中断 ADC使用中断来通知转换数据完毕的
      
       2、状态位置位；
            havedata=1；

       3、唤醒阻塞进程
            wake_up()
}

probe（）
{
      1、 读取中断号，注册中断处理函数

      2、读取寄存器的地址，ioremap

      3、字符设备的操作
}		
		
		
添加设备树信息： 
		4、设备树中的节点编写
		[java] view plain copy
		fs4412-adc{  
			compatible = "fs4412,adc";  
			reg = <0x126C0000 0x20>;  
			interrupt-parent = <&combiner>;  
			interrupts = <10 3>;  
		};  
		

驱动：

#include <linux/module.h>  
#include <linux/device.h>  
#include <linux/platform_device.h>  
#include <linux/interrupt.h>  
#include <linux/fs.h>  
#include <linux/wait.h>  
#include <linux/sched.h>  
#include <asm/uaccess.h>  
#include <asm/io.h>  

static int major = 250;  
  
  
static wait_queue_head_t wq;  
static int have_data = 0;  
static int fs4412_adc;  
static struct resource *res1;  
static struct resource *res2;  
static void *fs4412_adc_base;  
  
 #define  ADCCON    0x0000  
 #define  ADCDLY    0x0008  
 #define  ADCDAT    0x000C  
 #define  CLRINTADC  0x0018  
 #define  ADCMUX    0x001C


int flags = 1;
//#define flags 1 

#define DEBUG_PRINTK(msg,DEBUG_FLAG) \
    do{ \
        if(!!DEBUG_FLAG) { \
            printk("---->%s--->%d\n",__func__,__LINE__);\
            printk(msg);\
		}\
    }while(0)

static  irqreturn_t fs4412_adc_handler(int irqno, void *dev)  
{  
    have_data = 1;   //唤醒条件
  
    printk("fs4412_adc_handler");  
    /*清中断*/  
    writel(0x12,fs4412_adc_base + CLRINTADC);  
    wake_up_interruptible(&wq);  
    return IRQ_HANDLED;  
}  
static int fs4412_adc_open (struct inode *inod, struct file *filep)  
{  
  	DEBUG_PRINTK("fs4412_adc_open",flags);
    return 0;  
}  

static ssize_t fs4412_adc_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)  
{  

    writel(0x3,fs4412_adc_base + ADCMUX);  
    writel(1<<0 | 1<<14 | 0X1<<16 | 0XFF<<6 ,fs4412_adc_base + ADCCON );  
  
    wait_event_interruptible(wq, have_data == 1);  
  
    /*read data 如果并& 0xfff,否则数据出错 */  
    fs4412_adc = readl(fs4412_adc_base+ADCDAT) & 0xfff;  

   //驱动层不进行运算，直接将数据拷贝给应用层
    if(copy_to_user(buf,&fs4412_adc,sizeof(int)))  
    {  
        return -EFAULT;  
    }  
	
    have_data = 0;  
	
    return len;  
}  


static  int fs4412_adc_release(struct inode *inode, struct file *filep)  
{  
	DEBUG_PRINTK("fs4412_adc_release",flags);
    return 0;  
}  

static struct file_operations  fs4412_adc_ops ={  
    .open    = fs4412_adc_open,  
    .release = fs4412_adc_release,  
    .read    = fs4412_adc_read,  
};  
  
  
static int fs4412_adc_probe(struct platform_device *pdev)  
{  
    int ret;  
    printk("match 0k \n");  
  
    res1 = platform_get_resource(pdev,IORESOURCE_IRQ, 0);  
    res2 = platform_get_resource(pdev,IORESOURCE_MEM, 0);   
         
    ret = request_irq(res1->start,fs4412_adc_handler,IRQF_DISABLED,"adc1",NULL);  
    fs4412_adc_base = ioremap(res2->start,res2->end-res2->start);  
    printk("res1->start :%#x. res2->start :%#x.\n",res1->start,res2->start);
	
    register_chrdev(major, "adc", &fs4412_adc_ops);  
    init_waitqueue_head(&wq);  
      
    return 0;  
}  
static int fs4412_adc_remove(struct platform_device *pdev)  
{  
    free_irq(res1->start,NULL);  
    free_irq(res2->start,NULL);    
    unregister_chrdev( major, "adc");  
    return 0;  
}  
  
static struct of_device_id fs4412_adc_id[]=  
{  
    {.compatible = "fs4412,adc" },  
};  
  
static struct platform_driver fs4412_adc_driver=  
{  
      
    .probe = fs4412_adc_probe,  
    .remove = fs4412_adc_remove,  
    .driver ={  
        .name = "bigbang",  
        .of_match_table = fs4412_adc_id,  
    },  
};  
  
static int fs4412_adc_init(void)  
{  
    printk("fs4412_adc_init");  
    return platform_driver_register(&fs4412_adc_driver);  
}  
static void fs4412_adc_exit(void)  
{  
    platform_driver_unregister(&fs4412_adc_driver);  
    printk("fs4412_adc_exit \n");  
    return;  
}  
MODULE_LICENSE("GPL");  
module_init(fs4412_adc_init);  
module_exit(fs4412_adc_exit);  


test.c 

#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdio.h>  
  
  
int main()  
{  
    int fd,len;  
    int adc;  
    fd = open("/dev/hello",O_RDWR);  
    if(fd<0)  
    {  
        perror("open fail \n");  
        return ;  
    }  
  
    while(1)  
    {  
        read(fd,&adc,4);  
		sleep(1);
		printf("adc%0.2f V \n",(1.8*adc)/4096);  
    }  
  
    close(fd);  
}  


Makefile 文件：

export ARCH=arm
export CROSS_COMPILE=arm-linux-

#KERNELDIR :=/lib/modules/$(shell uname -r)/build
KERNELDIR :=/home/fengjunhui/kernel/linux-3.14
PWD  :=$(shell pwd)

obj-m +=demo.o 

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gcc test.c -o app

install:
	cp app demo.ko ~/source/rootfs
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf app


测试结果： 

[root@fengjunhui ]:~$ insmod demo.ko 
[root@makeru drv]# insmod fsadc.ko 
[   13.940000] fs4412_adc_initmatch 0k 
[   13.940000] res1->start :0xa5. res2->start :0x126c0000.
[root@fengjunhui ]:~$ ./app 
open fail 
: No such file or directory
[root@fengjunhui ]:~$ mknod /dev/hello c 250 0
[root@fengjunhui ]:~$ ./app 
[   67.000000] ---->fs4412_adc_open--->50
[   67.000000] fs4412_adc_open
[   67.005000] fs4412_adc_handler
adc0.68 V [   68.005000] fs4412_adc_handler
adc0.68 V [   69.010000] fs4412_adc_handler
adc0.95 V [   82.045000] fs4412_adc_handler
adc1.13 V [   83.045000] fs4412_adc_handler
adc1.43 V [   84.050000] fs4412_adc_handler
adc1.61 V [   85.050000] fs4412_adc_handler
adc1.80 V [   86.055000] fs4412_adc_handler
adc1.80 V [   87.055000] fs4412_adc_handler
adc1.80 V [   88.060000] fs4412_adc_handler
adc1.61 V [   89.060000] fs4412_adc_handler
adc1.42 V [   90.065000] fs4412_adc_handler
adc1.13 V [   91.065000] fs4412_adc_handler
adc0.89 V [   92.070000] fs4412_adc_handler
adc0.45 V [   93.070000] fs4412_adc_handler
adc0.00 V [   94.075000] fs4412_adc_handler
adc0.00 V [   95.075000] fs4412_adc_handler
^C[   95.860000] ---->fs4412_adc_release--->79
[   95.860000] fs4412_adc_release
[root@fengjunhui ]:~$ rmmod demo
fs4412_adc_exit 


测试一个关键的调试函数：
[root@fengjunhui ]:~$ insmod demo.ko 
[  500.695000] fs4412_adc_initmatch 0k 
[root@fengjunhui ]:~$ ./app 
[  502.890000] ---->fs4412_adc_open--->51
[  502.895000] fs4412_adc_open
[  502.895000] fs4412_adc_handleradc0.38 V fs4412_adc_handler
adc0.38 V fs4412_adc_handler
^C
[  505.660000] ---->fs4412_adc_release--->80
[  505.665000] fs4412_adc_release





