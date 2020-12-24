






/*
1，头文件
2，驱动模块装载和卸载函数入口到声明
3，实现模块装载和卸载函数入口
4，GPL声明

*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/stat.h>

#include "math.h"


static int myvalue = 56;
static char *myname = "peter";

static int __init hello_drv_init(void)
{

	//一般做系统申请资源
	printk("-------%s-------------\n", __FUNCTION__);
	printk("name = %s, value = %d\n", myname, myvalue);


	printk("a+b = %d, a-b = %d\n", my_add(33,22), my_sub(44,22));
	
	return 0;
}

static void __exit hello_drv_exit(void)
{
	//一般做系统释放资源
	printk("-------%s-------------\n", __FUNCTION__);

}


module_init(hello_drv_init);
module_exit(hello_drv_exit);

MODULE_LICENSE("GPL");
module_param(myvalue, int, 0644);
module_param(myname, charp, S_IRUGO|S_IWUSR);






