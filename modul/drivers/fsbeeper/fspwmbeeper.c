#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>


unsigned int major = 0;
const char * modulename="pwmbeeper";
dev_t devnum = 0;
struct class  *cls = NULL;
struct device *dev = NULL;

#define GPD0CON       0x114000a0  
#define TIMER_BASE    0x139D0000             
#define TCFG0         0x0000                 
#define TCFG1         0x0004                              
#define TCON          0x0008               
#define TCNTB0        0x000C            
#define TCMPB0        0x0010


static unsigned int *gpd0con;  
static void *timer_base;  

typedef struct beep_desc{
	int beep;    //2 3 4 5
	int beep_state;  //0 or 1
	int tcnt;  //占空比
	int tcmp;  //调节占空比
}beep_desc_t;

#define mytype 'f'
#define BEEP_ON 	_IOW(mytype,0,beep_desc_t)
#define BEEP_OFF 	_IOW(mytype,1,beep_desc_t)
#define BEEP_FREQ 	_IOW(mytype,2,beep_desc_t)


char kbuf[] = {'1','2','3','4'};


void fs4412_beep_on(void)  
{
	writel (readl(timer_base +TCON ) | (0x1 <<0),timer_base +TCON ); 
}

void fs4412_beep_off(void)  
{
	writel (readl(timer_base +TCON ) | (~(1 << 0)),timer_base +TCON ); 
}

static void beep_freq(int beep_tcnt,int beep_tcmp)  
{  
	//tcnt决定了周期  tcnt 取值范围：
    writel (beep_tcnt, timer_base +TCNTB0  );  //计数值 100次
    //tcmp设置占空比  50 / 100 = 50%   综述：占空比 = tcmp / tcnt;  //决定声音的大小
    writel (beep_tcmp, timer_base +TCMPB0 );  //比较值 50次 
}  


int demo_open(struct inode *inode, struct file *filp)
{
	printk("--->%s--->%d.\n",__FUNCTION__,__LINE__);
	//fs4412_beep_on();
	return 0;
}

int demo_release(struct inode *inode, struct file *filp)
{
	printk("--->%s--->%d.\n",__FUNCTION__,__LINE__);
	fs4412_beep_off();
	return 0;
}

ssize_t demo_read(struct file *filp, char __user *usrbuf, size_t size, loff_t *offset)
{
	int bytes = 0;
	printk("---->%s--->%d\n",__func__,__LINE__);

	bytes =	copy_to_user(usrbuf,kbuf,4);
	if(bytes > 0){
		printk("copy_to_user failed!\n");
	}
	
	return 0;
}

ssize_t demo_write (struct file *filp, const char __user *usrbuf, size_t size, loff_t *offset)
{
	int bytes = 0;
	printk("---->%s--->%d\n",__func__,__LINE__);

	bytes = copy_from_user(kbuf,usrbuf,4);
	if(bytes > 0){
		printk("copy_from_user failed\n");
		return -1;
	}
	printk("copy_from_user usrbuf:%c\n",kbuf[0]);

	return 0;
}

long demo_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	beep_desc_t *beeper = (beep_desc_t *)args;
	printk("---->%s--->%d\n",__func__,__LINE__);

    switch(cmd)  
    {  
        case BEEP_ON:  
            fs4412_beep_on();  
            break;  
        case BEEP_OFF:  
            fs4412_beep_off();  
            break;  
        case BEEP_FREQ:  
            beep_freq(beeper->tcnt,beeper->tcmp);  
            break;  
        default :  
            return -EINVAL;  
    }  
	
	return 0;
}


const struct file_operations  fops={
	.open = demo_open,
	.read = demo_read,
	.write = demo_write,
	.unlocked_ioctl = demo_ioctl,
	.release = demo_release,
};

void fs4412_beep_init(void)
{
	gpd0con = ioremap(GPD0CON,4);
	timer_base = ioremap(TIMER_BASE,4);
	
	//一级分频:PCLK/(249 + 1) = 100MHZ/250 = 400000 
	//PWM.TCFG0 = 199;
	writel ((readl(gpd0con)&~(0xf<<0)) | (0x2<<0),gpd0con); // GPD0_0 : TOUT_0 
    writel ((readl(timer_base +TCFG0  )&~(0xff<<0)) | (0Xf9 <<0),timer_base +TCFG0); //设置默认值0XF9 = 249分频
    writel ((readl(timer_base +TCFG1 )&~(0xf<<0)) | (0x2<<0),timer_base +TCFG1 );  //4分频 

	
    //调整后的PWM波形 100000 / (tcnt)100 = 1000hz （20 - 20000）hz 周期1ms //设置频率 -声音的高低
	//tcnt决定了周期  tcnt 取值范围：
    writel (100, timer_base +TCNTB0  );  //计数值 100次
    //tcmp设置占空比  50 / 100 = 50%   综述：占空比 = tcmp / tcnt;  //决定声音的大小
    writel (80, timer_base +TCMPB0 );  //比较值 50次 
    
	writel (readl(timer_base +TCON ) | (0x1 <<3),timer_base +TCON ); //设置自动重装载
	writel (readl(timer_base +TCON ) | (0x1 <<1),timer_base +TCON );  //设置手动重装
	writel (readl(timer_base +TCON ) & (~(1 << 1)),timer_base +TCON );//清除手动记载

}

static int __init  demo_init(void)
{
	printk("--->%s--->%d.\n",__FUNCTION__,__LINE__);

	major = register_chrdev(0, modulename, &fops);
	if(major <= 0){
		printk("register_chrdev failed.\n");
		return -1;
	}
	devnum = MKDEV(major, 0);
	printk(KERN_INFO "major :%d.\n",major);

	cls = class_create(THIS_MODULE, "fsbeeper");
	if(cls == NULL){
		printk("class_create failed");
		return -1;
	}

	dev = device_create(cls, NULL, devnum,NULL, "fsbeeper%d",0);
	if(dev == NULL){
		printk("device_create failed.\n");
		return -1;
	}
	fs4412_beep_init();

	return 0;
}


void fs4412_beep_unioremap(void)
{
	iounmap(gpd0con);
	iounmap(timer_base);
}


static void __exit  demo_exit(void)
{
	printk("--->%s--->%d.\n",__FUNCTION__,__LINE__);

	fs4412_beep_unioremap();
	
	device_destroy(cls,devnum);
	class_destroy(cls);
	unregister_chrdev(major, modulename);
	
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");





