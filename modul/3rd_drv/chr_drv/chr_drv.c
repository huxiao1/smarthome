#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <asm/uaccess.h>
#include <asm/io.h>


//物理地址
#define GPX2_CON 0x11000C40  
#define GPX2_SIZE 8

//存放虚拟地址的指针
volatile unsigned long *gpx2conf;
volatile unsigned long *gpx2dat;

static unsigned int dev_major = 250;
static struct class *devcls;
static struct device *dev;


static int kernel_val = 555;


//  read(fd, buf, size); buf是用户空间的
ssize_t chr_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
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


ssize_t chr_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	printk("-------%s-------\n", __FUNCTION__);


	int ret;
	int value;  //存储从用户空间读来的数据

	ret = copy_from_user(&value,  buf, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		return -EFAULT;
	}

	if(value){
		*gpx2dat |= (1<<7);
	}else{
		*gpx2dat &= ~(1<<7);
	}
	
	
	return 0;
	
}


int chr_drv_open(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}


int chr_drv_close(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}

const struct file_operations my_fops = {
	.open = chr_drv_open,
	.read = chr_drv_read,
	.write = chr_drv_write,
	.release = chr_drv_close,
};

static int __init chr_dev_init(void)
{

	int ret;
	
	ret = register_chrdev(dev_major, "chr_dev_test", &my_fops);
	if(ret == 0){
		printk("register ok\n");
	}else{
		printk("register failed\n");
		return -EINVAL;
	}


	devcls = class_create(THIS_MODULE, "chr_cls");

	// /dev/chr2
	dev = device_create(devcls, NULL, MKDEV(dev_major, 0), NULL, "chr2");

	//对地址进行映射,返回给虚拟地址
	gpx2conf = ioremap(GPX2_CON, GPX2_SIZE);
	
	gpx2dat = gpx2conf + 1;    //+1实际上是长度加4

	//配置gpio功为输出功能  28-31位
	*gpx2conf &= ~(0xf<<28);
	*gpx2conf |= (0x1<<28);

	return 0;
}

static void __exit chr_dev_exit(void)
{
	iounmap(gpx2conf);
	device_destroy(devcls,  MKDEV(dev_major, 0));
	class_destroy(devcls);
	unregister_chrdev(dev_major, "chr_dev_test");

}


module_init(chr_dev_init);
module_exit(chr_dev_exit);
MODULE_LICENSE("GPL");












