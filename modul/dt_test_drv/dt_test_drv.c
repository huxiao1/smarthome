#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

#define U32_DATA_LEN 4

static  int is_good;
static int irqno;

irqreturn_t key_irq_handler(int irqno, void *devid)
{
	printk("------------------------key pressed \n");
	return IRQ_HANDLED;
}


static int __init dt_drv_init(void)
{
	/*
		 test_nod@12345678{
                compatible = "farsight,test";
                reg = <0x12345678 0x24
                        0x87654321 0x24>;
                testprop,mytest;
                test_list_string = "red fish","fly fish", "blue fish";
                 interrupt-parent = <&gpx1>;
                interrupts = <1 4>;    <2 2> 上升沿和下降沿都有效

        };
	*/

	// 在代码中获取节点的所有信息
	//1.先把节点获取到
	struct device_node *np = NULL;
	
	np = of_find_node_by_path("/i2c@138B0000");
	if(np)
		{
			printk("find test node ok!\n");
			printk("node name = %s\n", np->name);
			printk("node full name = %s\n", np->full_name);
			
		}else{
			printk("find node failed!\n");
			
			}
	//2.获取到节点的属性
	struct property *prop = NULL;
	prop = of_find_property(np, "compatible", NULL);
	if(prop)
	{
		printk("find compatible ok\n");
		printk("compatible value = %s\n", prop->value);
		printk("compatible name = %s\n", prop->name);
	}else{
		printk("find compatible failed\n");

	}

	if(of_device_is_compatible(np, "farsight,test"))
	{
		printk("we have a compatible named farsight,test\n");
	}

	//3.1读取到属性中的整数的数组
	u32 regdata[U32_DATA_LEN];
	int ret;
	
	ret = of_property_read_u32_array(np, "reg", regdata, U32_DATA_LEN);
	if(!ret)
	{
		int i;
		for(i=0; i<U32_DATA_LEN; i++)
			printk("----regdata[%d] = 0x%x\n", i,regdata[i]);
		
	}else{
		printk("get reg data failed\n");
	}

	//3.2读取到属性中的字符串的数组
	const char *pstr[3];

	int i;
	for(i=0; i<3; i++)
	{
		ret = of_property_read_string_index(np, "test_list_string", i, &pstr[i]);
		if(!ret)
		{
			printk("----pstr[%d] = %s\n", i,pstr[i]);
		}else{
			printk("get pstr data failed\n");
		}
	}

	//3.3属性的值为空，实际可以用于设置标志
	if(of_find_property(np, "testprop,mytest", NULL))
	{
			is_good = 1;
			printk("is_good = %d\n", is_good);
	}

	//3.4获取到中断的号码
	irqno = irq_of_parse_and_map(np, 0);
	printk("-----irqno = %d\n", irqno);
	
	//3.5验证中断号码是否有效
	ret = request_irq(irqno, key_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "key_irq", NULL);
	if(ret)
	{
		printk("request_irq error\n");
		return -EBUSY;
	}

	
	return 0;
	
}



static void __exit dt_drv_exit(void)
{
	free_irq(irqno,  NULL);
}


module_init(dt_drv_init);
module_exit(dt_drv_exit);
MODULE_LICENSE("GPL");


