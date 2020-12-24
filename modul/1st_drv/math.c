
#include <linux/module.h>
#include <linux/init.h>



//不需要模块加载和卸载到入口声明，直接定义好一些封装的函数

int my_add(int a, int b)
{
	return a+b;
}

EXPORT_SYMBOL(my_add);

int my_sub(int a, int b)
{
	return a-b;
}

EXPORT_SYMBOL(my_sub);




MODULE_LICENSE("GPL");

