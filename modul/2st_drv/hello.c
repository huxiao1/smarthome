#include<linux/init.h>
#include<linux/module.h>
#include<linux/stat.h>
#include "math.h"


static int myvalue = 56;
static char *myname = "huxiao";


static int __init hello_drv_init(void)
{
	printk("-----------%s------------\n",__FUNCTION__);
	printk("name = %s,value = %d\n",myname,myvalue);
	printk("a + b = %d,a - b = %d\n",my_add(1,2),my_sub(2,1));
	//一般做系统申请资源
	return 0;
}


static void __exit hello_drv_exit(void)
{
	//一般做系统释放资源
	printk("-----------%s------------\n",__FUNCTION__);

	
}




module_init(hello_drv_init);
module_exit(hello_drv_exit);

MODULE_LICENSE("GPL");


module_param(myvalue, int, 0644);
module_param(myname, charp, 0644);

















