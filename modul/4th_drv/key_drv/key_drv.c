#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/poll.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#define GPXCON_REG 0x11000c20
#define KEY_ENTER 28

//设计一个描述按键数据的对象
struct key_event{
	int code;     //按键的类型
	int value;    //表示状态 按下还是抬起
};

//设计一个全局设备对象，描述按键信息
struct key_desc{
	unsigned int dev_major;
	struct class *cls;
	struct device *dev;
	int irqno;
	void *reg_base;
	struct key_event event;
	wait_queue_head_t wq_head;
	int key_state;   //表示是否有数据
}*key_dev;


ssize_t key_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	//printk("-------%s-------\n", __FUNCTION__);
	//如果当前是非阻塞模式，并且没有数据，立马返回一个出错码
	
	if(filp->f_flags & O_NONBLOCK && !key_dev->key_state)
		//return -EAGAIN;
		printk("1\n");
	
	
	int ret;
	//在需要等待(没有数据)的时候，进行休眠
	wait_event_interruptible(key_dev->wq_head, key_dev->key_state);   //状态位为0的话阻塞等待,有数据则进入中断
	
	ret = copy_to_user(buf,&key_dev->event, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		return -EFAULT;
	}

	//传递给用户数据后要将数据清除掉
	memset(&key_dev->event,0,sizeof(key_dev->event));
	key_dev->key_state =0;
	
	return 0;
}


ssize_t key_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
	
}


int key_drv_open(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}


int key_drv_close(struct inode *inode, struct file *filp)
{
	printk("-------%s-------\n", __FUNCTION__);
	return 0;
}

unsigned int key_drv_poll(struct file *filp, struct poll_table_struct *pts)
{
        
	// 返回一个mask值
	unsigned int mask;
    // 调用poll_wait,将当前到等待队列注册到系统中
    poll_wait(filp, &key_dev->wq_head, pts);
        
    // 1，当没有数据到时候返回一个0
    if(!key_dev->key_state)
        mask = 0;


    // 2，有数据返回一个POLLIN
    if(key_dev->key_state)
        mask |= POLLIN;

    return mask;
}


const struct file_operations key_fops = {
	.open = key_drv_open,
	.read = key_drv_read,
	.write = key_drv_write,
	.release = key_drv_close,
	.poll = key_drv_poll,
};
	

irqreturn_t key_irq_handler(int irqno,void *devid)
{
	printk("---------------%s--------------\n",__FUNCTION__);
	int value = readl(key_dev->reg_base + 4) & (1<<2);
	if(value)   //抬起
		{
			printk("key3 up\n");
			key_dev->event.code = KEY_ENTER;
			key_dev->event.value = 0;
		}
	else{
			printk("key3 pressed\n");
			key_dev->event.code = KEY_ENTER;
			key_dev->event.value = 1;
		}
	//有数据    需要去唤醒整个等待队列
	wake_up_interruptible(&key_dev->wq_head);
	//设置标志位
	key_dev->key_state = 1;
	
	
	return IRQ_HANDLED;
}


int get_irqno_from_node(void)
{
	//先获取到设备树当中的结点
	struct device_node *np = of_find_node_by_path("/key_int_node");
	if(np)
		{
			printk("find node OK!\n");
		}
	else{
			printk("find node failed!\n");
		}

	//通过结点获取到终端号码
	int irqno = irq_of_parse_and_map(np, 0);
	printk("irqno == %d\n",irqno);
	return irqno;
}

static int __init key_drv_init(void)
{

	// 1，设定一个全局的设备对象
    key_dev = kzalloc(sizeof(struct key_desc),  GFP_KERNEL);
    
    // 2,申请主设备号
    key_dev->dev_major = register_chrdev(0, "key_drv", &key_fops);


    // 3,创建设备节点文件
    key_dev->cls = class_create(THIS_MODULE, "key_cls");
    key_dev->dev = device_create(key_dev->cls, NULL,MKDEV(key_dev->dev_major,0), NULL, "key%d",3);

	//4.硬件的初始化  地址映射或者中断申请
	int ret;
	//演示如何获取到中断号
	
	key_dev->irqno = get_irqno_from_node();

	//a.硬件获取数据 GPX1     放在这里是因为如果放在中断后面初始执行中断函数里面的reg_base指针是NULL
	key_dev->reg_base = ioremap(GPXCON_REG, 8);
	
	ret = request_irq(key_dev->irqno, key_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "key3-eint10", NULL);
	if(ret != 0)
		{
			printk("request_irq_err!\n");
			return -EBUSY;
		}

	//初始化等待队列头
	init_waitqueue_head(&key_dev->wq_head);
	
	
	
	return 0;
}

static void __exit key_drv_exit(void)
{
	iounmap(key_dev->reg_base);
	free_irq(key_dev->irqno, NULL);
	device_destroy(key_dev->cls,MKDEV(key_dev->dev_major, 0));
	class_destroy(key_dev->cls);
	unregister_chrdev(key_dev->dev_major, "key_drv");
	kfree(key_dev);
}


















module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");




