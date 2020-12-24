/*
设备树文件：
   key_int_node{
        compatible = "test_key";
		interrupt-parent = <&gpx1>;
        interrupts = <2 0>;
	};
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>

#include <asm/io.h>


#define KEY_NUMS 3

//设计一个对象出来
struct key_desc{
	char *name;
	int irqno;
	int key_code;
	int gpionum;
	void *reg_base;
	struct device_node *cnp;// 可以随时去获取节点各个信息
}all_key[KEY_NUMS];



struct input_dev *inputdev;

void get_all_child_from_node(void)
{
	// 获取到设备树中到节点
	struct device_node *np = of_find_node_by_path("/key_int_node");
	if(np){
		printk("find node ok\n");
	}else{
		printk("find node failed\n");
	}

	struct device_node *cnp;
	struct device_node *prev = NULL;

	int i = 0;

	do{
			//获取到子节点
			cnp = of_get_next_child(np, prev);
			if(cnp != NULL){
				all_key[i++].cnp = cnp;//将当前的节点记录下来
			}

			prev = cnp; //把当前的设置位prev

		}while(of_get_next_child(np, prev) != NULL);

}


irqreturn_t  input_key_irq_handler(int irqno, void *devid)
{
	printk("-------%s-------------\n", __FUNCTION__);

	//区分不同的按键
	struct key_desc *pdesc = (struct key_desc *)devid;

	int gpionum = of_get_named_gpio(pdesc->cnp, "gpio", 0);
	
	
	//直接通过gpio获取按键状态
	int value = gpio_get_value(gpionum);
	
	if(value){//抬起
		
		input_report_key(inputdev, pdesc->key_code, 0);
		input_sync(inputdev);//上报数据结束
			
	}else{
		input_report_key(inputdev, pdesc->key_code, 1);
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

	get_all_child_from_node();

	//添加设备信息--/sys/class/input/eventx/device/
	inputdev->name = "simple input key";
	inputdev->phys = "key/input/input0";
	inputdev->uniq = "simple key0 for 4412";
	inputdev->id.bustype = BUS_HOST;
	inputdev->id.vendor =0x1234 ;
	inputdev->id.product = 0x8888;
	inputdev->id.version = 0x0001;

	//当前设备能够产生按键数据--将某个bit置1
	__set_bit(EV_KEY,  inputdev->evbit);
	//表示当前设备能够产生power按键
	//__set_bit(KEY_POWER, inputdev->keybit);
	//另外一种设置bit的方式

	int i;
	for(i=0; i<KEY_NUMS; i++)
	{
		//设置keybit，支持哪些按键
		// 按键值从设备设备树
		int code;
		struct device_node *cnp = all_key[i].cnp;
		
		of_property_read_u32(cnp,"key_code", &code);
		__set_bit(code, inputdev->keybit);
		all_key[i].key_code = code; //先记录下来


		int irqno;
		irqno = irq_of_parse_and_map(cnp, 0);
		all_key[i].irqno = irqno;//先记录下来

		char *key_name ;
		of_property_read_string(cnp, "key_name",  &key_name);
		all_key[i].name = key_name;

		printk("name = %s, code = %d,irqno = %d\n",
						key_name, code,irqno);	

		int irqflags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING;
		
		ret = request_irq(irqno, input_key_irq_handler, irqflags, 
					key_name, &all_key[i]);
		if(ret != 0)
		{
			printk("request_irq error\n");
			goto err_0;
		}
		
	}

	ret = input_register_device(inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device error\n");
		goto err_0;
	}

	return 0;

err_1:
	input_unregister_device(inputdev);
err_0:
	input_free_device(inputdev);
	return ret;
}



static void __exit simple_input_exit(void)
{
	int i;
	for(i=0; i<KEY_NUMS; i++)
		free_irq(all_key[i].irqno, &all_key[i]);
		
	input_unregister_device(inputdev);
	input_free_device(inputdev);
}


module_init(simple_input_init);
module_exit(simple_input_exit);
MODULE_LICENSE("GPL");


