
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include "chrdev.h"

unsigned int major = 0;  //动态加载
//unsigned int major = 500;

const char * name = "demoname";

char kbuf[4] = {1,2,3,4};
int numlen = ARRAY_SIZE(kbuf);

struct class * cls;
struct device * dev;


#define GPX2CON 0x11000C40
#define GPX2_7DAT 0x11000c44
#define GPX1CON 0x11000c20
#define GPX1DAT 0x11000c24
#define GPF3CON 0x114001e0
#define GPF3DAT 0x114001e4

void __iomem * gpx2con_vir;
void __iomem * gpx2dat_vir;
void __iomem * gpx1con_vir;
void __iomem * gpx1dat_vir;
void __iomem * gpf3con_vir;
void __iomem * gpf3dat_vir;

int demo_open(struct inode *inode, struct file *filp)
{
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	return 0;
}

int demo_close(struct inode *inode, struct file *filp)
{
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	return 0;
}

ssize_t demo_read(struct file *filp, char __user * usrbuf, size_t size, loff_t *offset)
{
	int rbytes;
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	rbytes = copy_to_user(usrbuf, kbuf, 4);
	if(rbytes > 0){
		printk("copy failed bytes :%d.\n",rbytes);
	}
	printk("copy success bytes :%d.\n",rbytes);
	
	return 0;

}

ssize_t demo_write(struct file *filp, const char __user *usrbuf, size_t size, loff_t *offset)
{
	int i = 0;
	int wbytes ;
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);
	
	wbytes = copy_from_user(kbuf, usrbuf,4);
	if(wbytes > 0){
		printk("copy failed bytes :%d.\n",wbytes);
	}
	for(i = 0;i < 4;i ++){
		printk("buf[%d] :%d.\n",i,kbuf[i]);
	}

	printk("copy success bytes :%d.\n",wbytes);	

	return 0;
}

long demo_ioctl(struct file *filp, unsigned int  cmd, unsigned long  args)
{	
	int lednum = 0;
	led_desc_t *led = (led_desc_t *)args;
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	switch (cmd){
	case FSLEDON:  
		lednum = led->which;
		if(lednum == 2){
			printk("fsled %d on.\n",lednum);
			writel((readl(gpx2dat_vir) | 0x1 << 7),gpx2dat_vir);
			//控制硬件
		}else if(lednum == 3){
			writel(readl(gpx1dat_vir) | (0x1 << 0),gpx1dat_vir);
			printk("fsled %d on.\n",lednum);
		}else if(lednum == 4){
			writel(readl(gpf3dat_vir) | (0x1 << 4),gpf3dat_vir);
			printk("fsled %d on.\n",lednum);
		}else if(lednum == 5){
			writel(readl(gpf3dat_vir) | (0x1 << 5),gpf3dat_vir);
			printk("fsled %d on.\n",lednum);
		}
		break;
	case FSLEDOFF:
		lednum = led->which;
		if(lednum == 2){
			writel((readl(gpx2dat_vir) & ~(0x1 << 7)),gpx2dat_vir);
			printk("fsled %d off.\n",lednum);
		}else if(lednum == 3){
			writel(readl(gpx1dat_vir)&~(0x1 << 0),gpx1dat_vir);
			printk("fsled %d off.\n",lednum);
		}else if(lednum == 4){
			writel(readl(gpf3dat_vir)&~(0x1 << 4),gpf3dat_vir);
			printk("fsled %d off.\n",lednum);
		}else if(lednum == 5){
			writel(readl(gpf3dat_vir)&~(0x1 << 5),gpf3dat_vir);
			printk("fsled %d off.\n",lednum);
		}
		break;
	default:
		printk("default :....\n");
		break;
	}

	return 0;
}

const struct file_operations fops = {
	.open = demo_open,
	.release = demo_close,
	.read = demo_read,
	.write = demo_write,
	.unlocked_ioctl = demo_ioctl,
};	


void gpio_led_ioremap(void)
{
	gpx2con_vir = ioremap(GPX2CON, 4);
	gpx2dat_vir = gpx2con_vir +4;

	gpx1con_vir = ioremap(GPX1CON,4);
	gpx1dat_vir = gpx1con_vir + 4;
	
	gpf3con_vir = ioremap(GPF3CON,4);
	gpf3dat_vir = gpf3con_vir + 4;
	
	writel((readl(gpx2dat_vir) | 0x0 << 7),gpx2dat_vir);
	writel(( readl(gpx2con_vir) & (~(0xF << 28))) | 0x1 << 28,gpx2con_vir);
	
	writel((readl(gpx1con_vir) & ~(0XF<< 0))| (0x1 << 0),gpx1con_vir);
	writel((readl(gpf3con_vir) & ~(0XFF<< 16 ))| (0x11 << 16),gpf3con_vir);
}

void gpio_led_iounmap(void)
{
	iounmap(gpx2con_vir);
	iounmap(gpx1con_vir);
	iounmap(gpf3con_vir);
}


static int __init demo_init(void)
{	
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	major = register_chrdev(0,  name, &fops);
	if(major <  0){
		printk("register_chrdev failed.\n");
	}
	printk("major :%d.\n",major);

	cls = class_create(THIS_MODULE, "chardev");
	if(cls == NULL){
		goto err0;
	}
	dev = device_create(cls,NULL,MKDEV(major, 0),NULL, "fsled%d",0);
	if(dev == NULL){
		printk("device_create failed.\n");
		goto err0;
	}

	//硬件操作
	gpio_led_ioremap();

	return 0;	
	
err0:
	unregister_chrdev(major, name);
	return -1;
}

static void __exit demo_exit(void)
{
	printk("-----%s-----%d.\n",__FUNCTION__,__LINE__);

	gpio_led_iounmap();
	device_destroy(cls,MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major,name);
}


//模块的三要素：
module_init(demo_init);  //入口函数
module_exit(demo_exit);//出口函数
MODULE_LICENSE("GPL");//GPL









