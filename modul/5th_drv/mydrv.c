#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include "dev_info.h"


extern struct bus_type mybus;
struct mydev_desc *pdesc;

int mydrv_probe(struct device *dev)
{
	printk("-----------%s-------------\n",__FUNCTION__);

	pdesc = (struct mydev_desc *)dev->platform_data;

	printk("name = %s\n",pdesc->name);
	printk("name = %d\n",pdesc->irqno);

	unsigned long *paddr = ioremap(pdesc->addr,8);
	
	return 0;
}

int mydrv_remove(struct device *dev)
{
	printk("-----------%s-------------\n",__FUNCTION__);
	return 0;
}


//实例化一个driver对象
struct device_driver mydrv = {
    .name = "fsdev_drv",
	.bus = &mybus,
	.probe = mydrv_probe,
	.remove = mydrv_remove,
};


static int __init mydrv_init(void)
{
	//将driver注册到总线中去
	int ret;
	ret = driver_register(&mydrv);
	if(ret < 0)
		{
			printk("drv register error!\n");
			return ret;
		}

	
	return 0;
}


static void __exit mydrv_exit(void)
{
	driver_unregister(&mydrv);

}








module_init(mydrv_init);
module_exit(mydrv_exit);
MODULE_LICENSE("GPL");



















