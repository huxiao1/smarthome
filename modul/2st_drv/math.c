
#include<linux/init.h>
#include<linux/module.h>


//不需要模块的装载和卸载的入口声明,直接定义好封装的函数

int my_add(int a,int b)
{
	return a+b;
}

EXPORT_SYMBOL(my_add);


int my_sub(int a,int b)
{
	return a-b;
}

EXPORT_SYMBOL(my_sub);












MODULE_LICENSE("GPL");








