#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <plat/irqs.h>
#include <asm/mach/irq.h>

#define GPIO_REG_BASE 0x11400000

#define GPF3_CON GPIO_REG_BASE + 0x01E0
#define GPF3_SIZE 24

#define GPX1_CON GPIO_REG_BASE + 0x0c20
#define GPX1_SIZE 24


//一个设备可能有多个资源:
struct resource led_res[] = {
	[0] = {
			.start = GPF3_CON,
			.end = GPF3_CON + GPF3_SIZE - 1,
			.flags = IORESOURCE_MEM,
		},
	[1] = {
			.start = GPX1_CON,
			.end = GPX1_CON + GPX1_SIZE - 1,
			.flags = IORESOURCE_MEM,
		},

	//有些设备也有中断资源，本驱动不用
	[2] = {
			.start = 67,
			.end = 67,
			.flags = IORESOURCE_IRQ,
		},
	
};


struct platform_device led_pdev = {
	.name = "exynos4412_led",     //用于做匹配
	.id = -1,
	.num_resources = ARRAY_SIZE(led_res),
	.resource = led_res,
};



static int __init plat_led_dev_init(void)
{
	//注册一个platform device
	return platform_device_register(&led_pdev);

}


static void __exit plat_led_dev_exit(void)
{
	platform_device_unregister(&led_pdev);

}












module_init(plat_led_dev_init);
module_exit(plat_led_dev_exit);
MODULE_LICENSE("GPL");

