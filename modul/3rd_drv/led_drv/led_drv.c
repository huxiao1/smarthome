#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/uaccess.h>
#include <asm/io.h>


//设计一类型用来描述设备信息
struct led_desc{
	unsigned int dev_major;   //设备号
	struct class *cls;
	struct device *dev;       //创建设备文件
	void *reg_virt_base;      //寄存器地址的基准值
};




//物理地址
#define GPX2_CON 0x11000C40
#define GPX2_SIZE 8

struct led_desc *led_dev;    //声明设备对象，全局的


static int kernel_val = 555;


//  read(fd, buf, size); buf是用户空间的
ssize_t led_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	printk("-------%s-------\n", __FUNCTION__);

	int ret;

	ret = copy_to_user(buf,  &kernel_val, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		return -EFAULT;
	}
	
	return 0;

}


ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	printk("-------%s-------\n", __FUNCTION__);


	int ret;
	int value;  //存储从用户空间读来的数据

	ret = copy_from_user(&value,  buf, count);
	if(ret > 0)
	{
		printk("copy_from_user error\n");
		return -EFAULT;
	}

	if(value){

		writel(readl(led_dev->reg_virt_base + 4) | (1<<7), led_dev->reg_virt_base + 4);
	}else{
		writel(readl(led_dev->reg_virt_base + 4) & ~(1<<7), led_dev->reg_virt_base + 4);
	}
	
	
	return 0;
	
}


int led_drv_open(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}


int led_drv_close(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}

const struct file_operations my_fops = {
	.open = led_drv_open,
	.read = led_drv_read,
	.write = led_drv_write,
	.release = led_drv_close,
};

static int __init led_dev_init(void)
{
	//实例化全局的设备对象
	led_dev = kmalloc(sizeof(struct led_desc),GFP_KERNEL);
	if(led_dev == NULL)
		{
			printk(KERN_ERR"malloc error\n");
			return -ENOMEM;
		}
	int ret;

	//动态分配设备号
	led_dev->dev_major = register_chrdev(0, "led_dev_test", &my_fops);
	if(led_dev->dev_major < 0){
		printk(KERN_ERR"led_dev->dev_major\n");
		ret = -ENODEV;
        goto err_0;
		
	}else{
		printk("register ok\n");
	}


	led_dev->cls = class_create(THIS_MODULE, "led_cls");
	if(IS_ERR(led_dev->cls))
		{
			printk(KERN_ERR"class create error\n");
			ret = PTR_ERR(led_dev->cls);       //指针出错的具体原因转换成一个出错码
			goto err_1;       //先释放主设备号再释放内存
		}

	
	// /dev/led2
	led_dev->dev = device_create(led_dev->cls, NULL, MKDEV(led_dev->dev_major, 0), NULL, "led%d",2);
	if(IS_ERR(led_dev->dev))
		{
			printk(KERN_ERR"device create error\n");
			ret = PTR_ERR(led_dev->dev);       //指针出错的具体原因转换成一个出错码
			goto err_2;       //先销毁类再释放主设备号再释放内存
		}


	
	//对地址进行映射,返回给虚拟地址
	led_dev->reg_virt_base = ioremap(GPX2_CON, GPX2_SIZE);
	if(led_dev->reg_virt_base == NULL)
		{
			printk(KERN_ERR"ioremap error\n");
			ret = -ENOMEM;
			goto err_3;
		}


	 // gpio的输出功能的配置
    u32 value = readl(led_dev->reg_virt_base);
    value &= ~(0xf<<28);
    value |= (0x1<<28);
    writel(value, led_dev->reg_virt_base); 

	return 0;

err_3:
		   device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));

err_2:
		   class_destroy(led_dev->cls);

err_1:
		   unregister_chrdev(led_dev->dev_major, "led_dev_test");

err_0:
           kfree(led_dev);
           return ret;
}


static void __exit led_dev_exit(void)
{
	iounmap(led_dev->reg_virt_base);
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	class_destroy(led_dev->cls);
	unregister_chrdev(led_dev->dev_major, "led_dev_test");
	kfree(led_dev);
}


module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");












