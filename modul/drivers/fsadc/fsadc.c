#include <linux/module.h>  
#include <linux/device.h>  
#include <linux/platform_device.h>  
#include <linux/interrupt.h>  
#include <linux/fs.h>  
#include <linux/wait.h>  
#include <linux/sched.h>  
#include <asm/uaccess.h>  
#include <asm/io.h>  
#include <linux/of.h>
#include <linux/device.h>

static wait_queue_head_t wq;  

unsigned int major = 0;
struct class  *cls = NULL;
struct device *dev = NULL;


static int have_data = 0;  
static int fs4412_adc;  
static struct resource *res1;  
static struct resource *res2;  
static void *fs4412_adc_base;  
  
 #define  ADCCON    0x0000  
 #define  ADCDLY    0x0008  
 #define  ADCDAT    0x000C  
 #define  CLRINTADC  0x0018  
 #define  ADCMUX    0x001C


int flags = 1;
//#define flags 1 

#define DEBUG_PRINTK(msg,DEBUG_FLAG) \
    do{ \
        if(!!DEBUG_FLAG) { \
            printk("---->%s--->%d\n",__func__,__LINE__);\
            printk(msg);\
		}\
    }while(0)

static  irqreturn_t fs4412_adc_handler(int irqno, void *dev)  
{  
    have_data = 1;   //唤醒条件
  
   printk("--->%s--->%d.\n",__func__,__LINE__);
    /*清中断*/  
    writel(0x12,fs4412_adc_base + CLRINTADC);  
    wake_up_interruptible(&wq);  
    return IRQ_HANDLED;  
}  
static int fs4412_adc_open (struct inode *inod, struct file *filep)  
{  
  	DEBUG_PRINTK("fs4412_adc_open",flags);
    return 0;  
}  

static ssize_t fs4412_adc_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)  
{  
    writel(0x3,fs4412_adc_base + ADCMUX);  
    writel(1<<0 | 1<<14 | 0X1<<16 | 0XFF<<6 ,fs4412_adc_base + ADCCON );  
  
    wait_event_interruptible(wq, have_data == 1);  
  
    /*read data 如果并& 0xfff,否则数据出错 */  
    fs4412_adc = readl(fs4412_adc_base+ADCDAT) & 0xfff;  

   //驱动层不进行运算，直接将数据拷贝给应用层
    if(copy_to_user(buf,&fs4412_adc,sizeof(int))) 
        return -EFAULT;  
    have_data = 0;  
	
    return len;  
}  


static  int fs4412_adc_release(struct inode *inode, struct file *filep)  
{  
	DEBUG_PRINTK("fs4412_adc_release",flags);
    return 0;  
}  

static struct file_operations  fs4412_adc_ops ={  
    .open    = fs4412_adc_open,  
    .release = fs4412_adc_release,  
    .read    = fs4412_adc_read,  
};  
  
  
static int fs4412_adc_probe(struct platform_device *pdev)  
{  
    printk("--->%s--->%d.\n",__func__,__LINE__);
  
    res1 = platform_get_resource(pdev,IORESOURCE_IRQ, 0);  
    res2 = platform_get_resource(pdev,IORESOURCE_MEM, 0);   

	fs4412_adc_base = ioremap(res2->start,res2->end - res2->start);   
	printk("res2->start,res2->end - res2->start :%#x.\n",res2->end - res2->start);
	
    request_irq(res1->start,fs4412_adc_handler,IRQF_DISABLED,"adc1",NULL); 
	printk("res1->start :%d.\n",res1->start);

	major = register_chrdev(0, "adc", &fs4412_adc_ops);  
	if(major <= 0){
		printk("register_chrdev failed.\n");
		return -1;
	}
	printk(KERN_INFO "major :%d.\n",major);

	cls = class_create(THIS_MODULE, "myadc");
	if(cls == NULL){
		printk("class_create failed");
		return -1;
	}

	dev = device_create(cls, NULL, MKDEV(major, 0),NULL, "fsadc%d",0);
	if(dev == NULL){
		printk("device_create failed.\n");
		return -1;
	}

    init_waitqueue_head(&wq);  
      
    return 0;  
}  
static int fs4412_adc_remove(struct platform_device *pdev)  
{  
    free_irq(res1->start,NULL);  
    free_irq(res2->start,NULL);    
    unregister_chrdev( major, "adc");  
    return 0;  
}  
  
static struct of_device_id myof_match_table[]=  {  
    {.compatible = "fs4412,adc" },  
};  
  
static struct platform_driver fs4412_adc_driver=  {  
    .probe = fs4412_adc_probe,  
    .remove = fs4412_adc_remove,  
    .driver ={  
        .name = "adc_demo",  
        .of_match_table = of_match_ptr(myof_match_table),  
    },  
};  
  
static int fs4412_adc_init(void)  
{  
    printk("--->%s--->%d.\n",__func__,__LINE__);
    return platform_driver_register(&fs4412_adc_driver);  
}  
static void fs4412_adc_exit(void)  
{  
	//printk("--->%s--->%d.\n",__func__,__LINE__);
    platform_driver_unregister(&fs4412_adc_driver);  
    return;  
}  
MODULE_LICENSE("GPL");  
module_init(fs4412_adc_init);  
module_exit(fs4412_adc_exit);  
