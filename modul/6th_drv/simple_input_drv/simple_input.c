#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>

#include <asm/io.h>


#define GPXCON_REG  0x11000C20

struct input_dev *inputdev;
int irqno;
void *reg_base;



int get_irqno_from_node(void)
{
	// 获取到设备树中到节点
	struct device_node *np = of_find_node_by_path("/key_int_node");
	if(np){
		printk("find node ok\n");
	}else{
		printk("find node failed\n");
	}

	// 通过节点去获取到中断号码
	int irqno = irq_of_parse_and_map(np, 0);
	printk("irqno = %d\n", irqno);
	
	return irqno;
}


irqreturn_t  input_key_irq_handler(int irqno, void *devid)
{
	printk("-------%s-------------\n", __FUNCTION__);

	//读取数据寄存器
	int value = readl(reg_base + 4) & (1<<2);
	if(value){//抬起
		input_event(inputdev, EV_KEY, KEY_POWER,  0);
		input_sync(inputdev);//上报数据结束
			
	}else{
		input_event(inputdev, EV_KEY, KEY_POWER,  1);
		input_sync(inputdev);//上报数据结束
	}


	return IRQ_HANDLED;
}


static int __init simple_input_init(void)
{

	//编写输入子系统代码
	/*
		1，分配一个input device对象
		2， 初始化input  device对象
		3，注册input device对象
	*/
	int ret;
	
	inputdev  =  input_allocate_device();
	if(inputdev == NULL)
	{
		printk(KERN_ERR "input_allocate_device error\n");
		return -ENOMEM;
	}


	//添加设备信息--/sys/class/input/eventx/device/
	inputdev->name = "simple input key";
	inputdev->phys = "key/input/input0";
	inputdev->uniq = "simple key0 for 4412";
	inputdev->id.bustype = BUS_HOST;
	inputdev->id.vendor =0x1234 ;
	inputdev->id.product = 0x8888;
	inputdev->id.version = 0x0001;


	//当前设备能够产生按键数据
	__set_bit(EV_KEY,  inputdev->evbit);
	//表示当前设备能够产生power按键
	__set_bit(KEY_POWER, inputdev->keybit);

	ret = input_register_device(inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device error\n");
		goto err_0;
	}
	//驱动硬件
	irqno = get_irqno_from_node();

	ret = request_irq(irqno, input_key_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, 
					"key3_eint10", NULL);
	if(ret != 0)
	{
		printk("request_irq error\n");
		goto err_1;
	}

	reg_base  = ioremap(GPXCON_REG, 8);
	
	return 0;

err_1:
	input_unregister_device(inputdev);
err_0:
	input_free_device(inputdev);
	return ret;
}



static void __exit simple_input_exit(void)
{
	iounmap(reg_base);
	free_irq(irqno, NULL);
	input_unregister_device(inputdev);
	input_free_device(inputdev);
}


module_init(simple_input_init);
module_exit(simple_input_exit);
MODULE_LICENSE("GPL");


