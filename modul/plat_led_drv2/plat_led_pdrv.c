#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/ioport.h>


#include <plat/irqs.h>
#include <asm/mach/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>


//设计一个全局的设备对象
struct led_dev{
	int dev_major;
	struct class *cls;
	struct device *dev;
	struct resource *res;     //获取到的内存资源
	void *reg_base;     //物理地址映射之后的虚拟地址
}*samsung_led;

int led_pdrv_open(struct inode *inode,struct file *filp)
{
	printk("----------%s---------\n",__FUNCTION__);
	return 0;
}

int led_pdrv_close(struct inode *inode,struct file *filp)
{
	printk("----------%s---------\n",__FUNCTION__);
	return 0;
}

int led_pdrv_write(struct file *filp,const char __user *buf,size_t count,loff_t *fpos)
{
	int val,ret;

	ret = copy_from_user(&val,buf,count);
	if(ret > 0)
		{
			printk("error\n");
			return -EFAULT;
		}
	if(val)
		{
			writel((readl(samsung_led->reg_base + 4)) | (0x3 << 4) ,samsung_led->reg_base + 4);
		}
	else
		{
			writel((readl(samsung_led->reg_base + 4)) & ~(0x3 << 4) ,samsung_led->reg_base + 4);
		}
	return count;
}	



const struct file_operations led_fops = {
	.open = led_pdrv_open,
	.release = led_pdrv_close,
	.write = led_pdrv_write,
};

int led_pdrv_probe(struct platform_device *pdev)
{
	printk("----------%s---------\n",__FUNCTION__);

	samsung_led = kzalloc(sizeof(struct led_dev),GFP_KERNEL);
	if(samsung_led == NULL)
		{
			printk("error\n");
			return -ENOMEM;
		}
	samsung_led->dev_major = register_chrdev(0,"led_drv", &led_fops);

	samsung_led->cls = class_create(THIS_MODULE, "led_new_cls");

	samsung_led->dev = device_create(samsung_led->cls,NULL,MKDEV(samsung_led->dev_major,0),NULL,"led0");

	samsung_led->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	int irqno = platform_get_irq(pdev, 0);
	printk("-------------irqno = %d\n",irqno);
	
	samsung_led->reg_base = ioremap(samsung_led->res->start, resource_size(samsung_led->res));

	//对寄存器进行配置--输出功能
	writel((readl(samsung_led->reg_base) & ~(0xff<<16)) | (0x11<<16) ,samsung_led->reg_base);
	
	return 0;
}


int led_pdrv_remove(struct platform_device *pdev)
{
	printk("-------%s--------\n",__FUNCTION__);
	iounmap(samsung_led->reg_base);
	device_destroy(samsung_led->cls,MKDEV(samsung_led->dev_major, 0));
	class_destroy(samsung_led->cls);
	unregister_chrdev(samsung_led->dev_major,"led_drv");
	
	kfree(samsung_led);
	return 0;
}


const struct platform_device_id led_id_table[] = {
	{"exynos4412_led",0x4444},
	{"s5pv210_led",0x2222},
};


struct platform_driver led_pdrv = {
	.probe = led_pdrv_probe,
	.remove = led_pdrv_remove,
	.driver = {
			.name = "samsung_led_drv",   //可以用于做匹配但这里不用
			//   /sys/bus/platform/drivers/samsung_led_drv 创建目录用
			
		},
	.id_table = led_id_table,
};



static int __init plat_led_drv_init(void)
{
	//注册一个platform driver
	return platform_driver_register(&led_pdrv);

}


static void __exit plat_led_drv_exit(void)
{
	platform_driver_unregister(&led_pdrv);

}












module_init(plat_led_drv_init);
module_exit(plat_led_drv_exit);
MODULE_LICENSE("GPL");



