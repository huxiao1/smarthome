#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/slab.h>



//封装设备信息
struct led_dev{
	int dev_major;
	struct class * cls;
	struct device *dev;
	struct resource * res;
	void *reg_base;
}*samsung_led;      //定义设备信息





ssize_t led_pdrv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fops)
{
	int ret;
	int value;
	
	ret = copy_from_user(&value, buf, count);

	if(ret){
		printk("copy_from_user error\n");
		return -EFAULT;
	}

	if(value){

		//数据寄存器置1
		writel(readl(samsung_led->reg_base+4) | (0x3<<4), samsung_led->reg_base+4);
		
	}else{
	
		writel(readl(samsung_led->reg_base+4) & ~(0x3<<4), samsung_led->reg_base+4);
	}

	return count;
}



int led_pdrv_open(struct inode *inode, struct file *filp)
{
	printk("---------%s---------\n",__FUNCTION__);

	return 0;
}


int led_pdrv_close(struct inode *inode, struct file *filp)
{

	printk("---------%s---------\n",__FUNCTION__);

	return 0;
}


const struct file_operations led_fops = {
	.write = led_pdrv_write,
	.open = led_pdrv_open,
	.release = led_pdrv_close,
};


//实现初始化
int led_pdrv_probe(struct platform_device *pdev)
{
	printk("---------%s---------\n",__FUNCTION__);

	samsung_led = kzalloc(sizeof(struct led_dev),GFP_KERNEL);

	if(samsung_led == NULL){

		printk("kzalloc error\n");
		return -ENOMEM;
	
	}

	samsung_led->dev_major = register_chrdev(0,"led_drv", &led_fops);

	samsung_led->cls = class_create(THIS_MODULE,"led_cls");

	samsung_led->dev = device_create(samsung_led->cls, NULL, MKDEV(samsung_led->dev_major, 0), NULL,"led0");

	//获取资源
	samsung_led->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	//地址映射
	samsung_led->reg_base = ioremap(samsung_led->res->start, resource_size(samsung_led->res));

	//设置寄存器状态，这里时输出状态
	writel((readl(samsung_led->reg_base) & ~(0xff<<16))| (0x11<<16), samsung_led->reg_base);

	return 0;
}



int led_pdrv_remove(struct platform_device *pdev)
{

	iounmap(samsung_led->reg_base);

	device_destroy(samsung_led->cls, MKDEV(samsung_led->dev_major, 0));

	class_destroy(samsung_led->cls);

	unregister_chrdev(samsung_led->dev_major, "led_drv");	

	kfree(samsung_led);

	return 0;
}



//用于匹配
const struct platform_device_id led_id_table[] = {

	{"exynos4412_led",0x4444},

	{"s5pv210_led",0x3333},

	{"e3c6410_led",0x2222},
};


struct platform_driver led_pdrv = {

	.probe = led_pdrv_probe,

	.remove = led_pdrv_remove,

	.driver = {
		.name = "samsung_led_drv",//可以用于做匹配，也可以不用于做匹配
	},

	.id_table = led_id_table,
};


static int __init plat_led_drv_init(void)
{

	return platform_driver_register(&led_pdrv);

}


static void __exit plat_led_drv_exit(void)
{

	platform_driver_unregister(&led_pdrv);

}




 
module_init(plat_led_drv_init);
module_exit(plat_led_drv_exit);



MODULE_LICENSE("GPL");
