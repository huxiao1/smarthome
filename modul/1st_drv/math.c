
#include <linux/module.h>
#include <linux/init.h>



//����Ҫģ����غ�ж�ص����������ֱ�Ӷ����һЩ��װ�ĺ���

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

