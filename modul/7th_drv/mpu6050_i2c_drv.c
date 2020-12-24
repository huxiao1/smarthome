#include "mpu6050.h"#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/i2c.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "mpu6050.h"

#define SMPLRT_DIV		0x19 //采样频率寄存器-25 典型值：0x07(125Hz)
									//寄存器集合里的数据根据采样频率更新
#define CONFIG			0x1A	//配置寄存器-26-典型值：0x06(5Hz)
										//DLPF is disabled（DLPF_CFG=0 or 7）
#define GYRO_CONFIG		0x1B//陀螺仪配置-27,可以配置自检和满量程范围
									//典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG		0x1C	//加速度配置-28 可以配置自检和满量程范围及高通滤波频率
										//典型值：0x01(不自检，2G，5Hz)
#define ACCEL_XOUT_H	0x3B //59-65,加速度计测量值 XOUT_H   重力加速度
#define ACCEL_XOUT_L	0x3C  // XOUT_L
#define ACCEL_YOUT_H	0x3D  //YOUT_H
#define ACCEL_YOUT_L	0x3E  //YOUT_L
#define ACCEL_ZOUT_H	0x3F  //ZOUT_H
#define ACCEL_ZOUT_L	0x40 //ZOUT_L---64
#define TEMP_OUT_H		0x41 //温度测量值--65
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43 //陀螺仪值--67，采样频率（由寄存器 25 定义）写入到这些寄存器  转角
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48 //陀螺仪值--72
#define PWR_MGMT_1		0x6B //电源管理 典型值：0x00(正常启用)

//-------------------------------------------------------------------------------------//
//设计一个全局的设备对象
struct mpu_sensor{
	int dev_major;
	struct device *dev;
	struct class *cls;
	struct i2c_client *client;//记录probe中client
}*mpu_dev;


//-----------------------------------------------------------------------//
//i2c的读和写
int mpu6050_write_bytes(struct i2c_client *client, char *buf, int count)
{

	int ret;
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = 0;
	msg.len = count;
	msg.buf = buf;
	
	ret = i2c_transfer(adapter, &msg,  1);    //i2c从设备,消息个数就一个成功返回1，否则出错码

	return ret==1?count:ret;

}

//读取方式太单一要先指定读哪个寄存器
int mpu6050_read_bytes(struct i2c_client *client, char *buf, int count)
{
	
		int ret;
		struct i2c_adapter *adapter = client->adapter;
		struct i2c_msg msg;
	
		msg.addr = client->addr;
		msg.flags = I2C_M_RD;
		msg.len = count;
		msg.buf = buf;
		
		ret = i2c_transfer(adapter, &msg,  1);
	
		return ret==1?count:ret;
}
//-----------------------------------------------------------------------//

//读取某个特定寄存器的地址，然后返回值
int mpu6050_read_reg_byte(struct i2c_client *client, char reg)
{
	// 先写寄存器的地址， 然后在读寄存器的值

		int ret;
		struct i2c_adapter *adapter = client->adapter;
		struct i2c_msg msg[2];

		char rxbuf[1];

		//写
		msg[0].addr = client->addr;
		msg[0].flags = 0;
		msg[0].len = 1;
		msg[0].buf = &reg;

		//读
		msg[1].addr = client->addr;
		msg[1].flags = I2C_M_RD;
		msg[1].len = 1;
		msg[1].buf = rxbuf;
		
		ret = i2c_transfer(adapter, msg,  2);
		if(ret < 0)
		{
			printk("i2c_transfer read error\n");
			return ret;
		}

		//读到的数据返回回去
		return rxbuf[0];

}


int mpu6050_drv_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int mpu6050_drv_close(struct inode *inode, struct file *filp)
{
	return 0;
}

long mpu6050_drv_ioctl (struct file *filp, unsigned int cmd, unsigned long args)
{
	union mpu6050_data data;


	switch(cmd){
		case IOC_GET_ACCEL:
			//读数据
			data.accel.x = mpu6050_read_reg_byte(mpu_dev->client, ACCEL_XOUT_L);
			data.accel.x |= mpu6050_read_reg_byte(mpu_dev->client, ACCEL_XOUT_H) << 8;

			data.accel.y = mpu6050_read_reg_byte(mpu_dev->client, ACCEL_YOUT_L);
			data.accel.y |= mpu6050_read_reg_byte(mpu_dev->client, ACCEL_YOUT_H) << 8;

			data.accel.z = mpu6050_read_reg_byte(mpu_dev->client, ACCEL_ZOUT_L);
			data.accel.z |= mpu6050_read_reg_byte(mpu_dev->client, ACCEL_ZOUT_H) << 8;
			break;
		case IOC_GET_GYRO:
			data.gyro.x = mpu6050_read_reg_byte(mpu_dev->client, GYRO_XOUT_L);
			data.gyro.x |= mpu6050_read_reg_byte(mpu_dev->client, GYRO_XOUT_H) << 8;


			data.gyro.y = mpu6050_read_reg_byte(mpu_dev->client, GYRO_YOUT_L);
			data.gyro.y |= mpu6050_read_reg_byte(mpu_dev->client, GYRO_YOUT_H) << 8;

			data.gyro.z= mpu6050_read_reg_byte(mpu_dev->client, GYRO_ZOUT_L);
			data.gyro.z |= mpu6050_read_reg_byte(mpu_dev->client, GYRO_ZOUT_H) << 8;
			break;
		case IOC_GET_TEMP:
			data.temp = mpu6050_read_reg_byte(mpu_dev->client, TEMP_OUT_L);
			data.temp |= mpu6050_read_reg_byte(mpu_dev->client, TEMP_OUT_H) << 8;
			break;
		default:
			printk("invalid cmd\n");
			return -EINVAL;
	}

	//将所有的数据交给用户
	if(copy_to_user((void __user * )args, &data, sizeof(data)) > 0)
		return -EFAULT;

	return 0;
}


const struct file_operations mpu6050_fops = {
	.open = mpu6050_drv_open,
	.release = mpu6050_drv_close,
	.unlocked_ioctl = mpu6050_drv_ioctl,
};



int mpu6050_drv_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("-----%s----\n", __FUNCTION__);

	/*
		申请设备号，实现fops
			创建设备文件
			通过i2c的接口去初始化i2c从设备
	*/

	mpu_dev = kzalloc(sizeof(struct mpu_sensor), GFP_KERNEL);   //先给设备对象分配空间
	
	mpu_dev->client = client;      //记录传来的client

	mpu_dev->dev_major = register_chrdev(0,"mpu_drv", &mpu6050_fops);   //申请设备号,0是为了自动分配

	mpu_dev->cls = class_create(THIS_MODULE, "mpu_cls");    //创建类

	mpu_dev->dev = device_create(mpu_dev->cls, NULL, MKDEV(mpu_dev->dev_major, 0),
				NULL, "mpu_sensor");            //创建设备节点


	//初始化
	char buf1[2] = {PWR_MGMT_1, 0x0};
	mpu6050_write_bytes(mpu_dev->client, buf1, 2);   //之前6050的client已经记录下来了

	char buf2[2] = {SMPLRT_DIV, 0x07};
	mpu6050_write_bytes(mpu_dev->client, buf2, 2);

	char buf3[2] = {CONFIG, 0x06};
	mpu6050_write_bytes(mpu_dev->client, buf3, 2);

	char buf4[2] ={GYRO_CONFIG, 0x18};
	mpu6050_write_bytes(mpu_dev->client, buf4, 2);

	char buf5[2] = {ACCEL_CONFIG, 0x01};
	mpu6050_write_bytes(mpu_dev->client, buf5, 2);
	
	return 0;

}


int mpu5060_drv_remove(struct i2c_client *client)
{
	printk("-----%s----\n", __FUNCTION__);
	device_destroy(mpu_dev->cls, MKDEV(mpu_dev->dev_major, 0));
	class_destroy(mpu_dev->cls);
	unregister_chrdev(mpu_dev->dev_major, "mpu_drv");
	kfree(mpu_dev);
	return 0;
}


const struct of_device_id  of_mpu6050_id[] = {
		{
			.compatible = "invensense,mpu6050",   //与设备树当中从设备信息一致
		},
};


const struct i2c_device_id mpu_id_table[] = {
		{"mpu6050_drv", 0x1111},
};

	
struct i2c_driver mpu6050_drv = {
	.probe = mpu6050_drv_probe,
	.remove = mpu5060_drv_remove,
	.driver = {
		.name = "mpu6050_drv",//随便写，/sys/bus/i2c/driver/mpu6050_drv创建用的
		.of_match_table = of_match_ptr(of_mpu6050_id),
	},
	
	.id_table = mpu_id_table,//非设备树情况下的匹配,在设备树的模式下不需要使用

};


static int __init mpu6050_drv_init(void)
{
	//1.构建i2c driver,并注册到i2c总线
	return i2c_add_driver(&mpu6050_drv);
	
}



static void __exit mpu6050_drv_exit(void)
{
	i2c_del_driver(&mpu6050_drv);
}


module_init(mpu6050_drv_init);
module_exit(mpu6050_drv_exit);
MODULE_LICENSE("GPL");


