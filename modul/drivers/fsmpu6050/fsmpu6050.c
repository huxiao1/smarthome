#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include<linux/ioport.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <asm/delay.h>
#include "common.h"
#include "mpu6050.h"


#define GPBCON 0x11400040
void __iomem *gpbcon_vir;

unsigned int  major ;
struct class *cls;
struct device *dev;


char kbuf[] = {'1','2','3','4'};

struct i2c_client *mpu_client;

char  mpu6050_read(char regaddr)
{
	char txbuf[1] = {regaddr};
	char rxbuf[1] = {0};
	
	struct i2c_msg  msgs[2] = {
		{mpu_client->addr,0,1,txbuf},
		{mpu_client->addr,1,1,rxbuf}
	};

	i2c_transfer(mpu_client->adapter,msgs,2);

	return rxbuf[0];
}

int  mpu6050_write(char reg,char val)
{
	char  txbuf[] ={reg,val};
	
	struct i2c_msg  msgs[] = {
		{mpu_client->addr,0,2,txbuf},
	};

	i2c_transfer(mpu_client->adapter,msgs,1);

	return  0;
}

int mpu6050_gpio_init(void)
{
    mpu6050_write(MPU_PWR_MGMT1_REG, 0x80);//复位MPU6050  
    udelay(2000);
    mpu6050_write(MPU_PWR_MGMT1_REG, 0x0);//唤醒MPU6050    
    	mpu6050_write(MPU_GYRO_CFG_REG, 0xF8);//陀螺仪传感器,±2000dps  
    mpu6050_write(MPU_ACCEL_CFG_REG, 0x19);//加速度传感器 2g

//	mpu6050_write(MPU_GYRO_CFG_REG, (0x3 << 3));//陀螺仪传感器,±2000dps  
//   mpu6050_write(MPU_ACCEL_CFG_REG, (0x0 << 3));//加速度传感器 2g

    //mpu6050_write(MPU_ACCEL_CFG_REG, 0x01);//唤醒MPU6050    
   
	mpu6050_write(MPU_SAMPLE_RATE_REG, 0x07);//设置采æ ·率50Hz       
    mpu6050_write(MPU_CFG_REG, 0x06);//唤醒MPU6050    
	
#if 1
    printk("who am i:0x%x\n", mpu6050_read(0x75)); 
#endif 
		
	return 0;
}


int mpu6050_open(struct inode *inode, struct file *filp)
{
	//硬件的初始化工作--收发数据的初始化
	printk("---->%s--->%d\n",__func__,__LINE__);
	return 0;
}

ssize_t dev_mpu6050_read(struct file *filp, char __user *usrbuf, size_t count, loff_t *offset)
{
	int bytes = 0;
	printk("---->%s--->%d\n",__func__,__LINE__);

	bytes =	copy_to_user(usrbuf,kbuf,4);
	if(bytes > 0){
		printk("copy_to_user failed!\n");
	}

	return 0;
}

ssize_t dev_mpu6050_write(struct file *filp, const char __user *usrbuf, size_t size, loff_t *offset)
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

long mpu6050_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	struct mpu6050_data data;

//	printk("---->%s--->%d\n",__func__,__LINE__);
	
	switch(cmd)
	{
		case MPU6050_GYRO:
			data.gyro.x =  mpu6050_read(MPU_GYRO_XOUTL_REG);	
			data.gyro.x |= mpu6050_read(MPU_GYRO_XOUTH_REG) << 8;	
			
			data.gyro.x =  mpu6050_read(MPU_GYRO_YOUTL_REG);	
			data.gyro.x |= mpu6050_read(MPU_GYRO_YOUTH_REG) << 8;	
			
			data.gyro.x =  mpu6050_read(MPU_GYRO_ZOUTL_REG);	
			data.gyro.x |= mpu6050_read(MPU_GYRO_ZOUTH_REG) << 8;	
			break;

		case MPU6050_ACCEL:
			data.accel.x =  mpu6050_read(MPU_ACCEL_XOUTL_REG);
			data.accel.x |= mpu6050_read(MPU_ACCEL_XOUTH_REG) << 8;
			
			data.accel.y =  mpu6050_read(MPU_ACCEL_YOUTL_REG);
			data.accel.y |= mpu6050_read(MPU_ACCEL_YOUTH_REG) << 8;
			
			data.accel.z =  mpu6050_read(MPU_ACCEL_ZOUTL_REG);
			data.accel.z |= mpu6050_read(MPU_ACCEL_ZOUTH_REG) << 8;
			break;

		case MPU6050_TEMP:
			data.temp = mpu6050_read(MPU_TEMP_OUTL_REG);
			data.temp |= mpu6050_read(MPU_TEMP_OUTH_REG) << 8;

			break;
		default:
			printk("sorry, cmd wrong !\n");
	}

	if (copy_to_user((void *)args, &data, sizeof(data)))  
		return -EFAULT;  

	return 0;
}

int mpu6050_close(struct inode *inode, struct file *filp)
{
	printk("---->%s--->%d\n",__func__,__LINE__);
	return 0;
}

const struct file_operations mpu6050_fops = {
	.owner = THIS_MODULE,
	.open  = mpu6050_open,
	.read  = dev_mpu6050_read,
	.write = dev_mpu6050_write,
	.unlocked_ioctl = mpu6050_ioctl,
	.release = mpu6050_close,
};


int mpu6050_probe(struct i2c_client *client, struct i2c_device_id *devid)
{
	printk("---->%s--->%d\n",__func__,__LINE__);
	mpu_client = client;
	
	printk("MPU6050 probe name.....%s\n",devid->name);
	printk("MPU6050 probe addr.....%#x\n",client->addr);
	
	major = register_chrdev(0,"mpu6050",&mpu6050_fops); 
	if(major <= 0){
		printk("register_chrdev failed!\n");
	}
	printk("register_chrdev success .major: %d\n",major);
	
	cls = class_create(THIS_MODULE,"mpu6050");
	if(cls == NULL){
		printk("class_create failed!\n");
	}

	dev = device_create(cls, NULL,MKDEV(major,0),NULL,"fsmpu60500");
	if(dev == NULL){	
		printk("device_create failed!\n");
	}

//	printk("mpu_6050 read who am i :0x%#x\n",mpu6050_read(0x75));
	
	mpu6050_gpio_init();

	return 0;
}

int mpu6050_remove(struct i2c_client *mpu_6050)
{
	printk("---->%s--->%d\n",__func__,__LINE__);
	return 0;
}

struct i2c_device_id mpu6050_id_table[] = {
	{},
	{},
};

struct of_device_id  mpu6050_device_id[] = {
	{.compatible = "invensense,mpu6050"},
	{},
};

struct i2c_driver mpu6050={
	.driver={
		.name = "mpu6050",
		.of_match_table = of_match_ptr(mpu6050_device_id),
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table =mpu6050_id_table,
	
};


static int __init mpu6050_init(void)
{
	printk("---->%s--->%d\n",__func__,__LINE__);

	if(i2c_add_driver(&mpu6050)){
		printk("driver_register failed\n");
		return -1;
	}
	return 0;
}


static void __exit mpu6050_exit(void)
{
//	printk("---->%s--->%d\n",__func__,__LINE__);
//	iounmap(gpbcon_vir);
	device_destroy(cls,MKDEV(major,0));
	class_destroy(cls);
	unregister_chrdev(major,"mpu6050");
	i2c_del_driver(&mpu6050);
}


module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_LICENSE("GPL");




#if 0

测试结果：

[root@fengjunhui ]:~$ insmod mpu6050.ko 
[ 1361.710000] ---->mpu6050_init--->240
[ 1361.710000] ---->mpu6050_probe--->180
[ 1361.715000] MPU6050 probe name.....(null)
[ 1361.720000] MPU6050 probe addr.....0x68
[ 1361.720000] register_chrdev success .major: 253
[ 1361.745000] who am i:0x68
[root@fengjunhui ]:~$ ./app
[ 4315.375000] ---->mpu6050_open--->87
open device succe[ 4315.585000] ---->dev_mpu6050_read--->94
[ 4315.590000] ---->dev_mpu6050_write--->107
[ 4315.595000] copy_from_user usrbuf:9
ss! fd: 3
usrbuf[2] : 3
----------------------------------accel-------------------
accel data: x = 001848, y = 065432, z = 015538
***********************************gyro*******************
gyro data: x = 000005, y = 000000, z = 000002
===================================temp===================
temp data: z = 063159
----------------------------------accel-------------------
accel data: x = 001860, y = 065430, z = 015514
***********************************gyro*******************
gyro data: x = 000005, y = 000000, z = 000002
===================================temp===================
temp data: z = 063159
----------------------------------accel-------------------
accel data: x = 001844, y = 065422, z = 015518
***********************************gyro*******************
gyro data: x = 000005, y = 000000, z = 000002
===================================temp===================
temp data: z = 063160
----------------------------------accel-------------------
accel data: x = 001856, y = 065422, z = 015508
***********************************gyro*******************
gyro data: x = 000005, y = 000000, z = 000002
===================================temp===================
temp data: z = 063156
----------------------------------accel-------------------
accel data: x = 001858, y = 065424, z = 015484
^C[ 4319.900000] ---->mpu6050_close--->165

[root@fengjunhui ]:~$ rmmod mpu6050
[ 4425.180000] ---->mpu6050_exit--->253
[ 4425.185000] ---->mpu6050_remove--->211


#endif 


















