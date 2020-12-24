#include <linux/init.h>
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


#define SMPLRT_DIV		0x19 //����Ƶ�ʼĴ���-25 ����ֵ��0x07(125Hz)
									//�Ĵ�������������ݸ��ݲ���Ƶ�ʸ���
#define CONFIG			0x1A	//���üĴ���-26-����ֵ��0x06(5Hz)
										//DLPF is disabled��DLPF_CFG=0 or 7��
#define GYRO_CONFIG		0x1B//����������-27,���������Լ�������̷�Χ
									//����ֵ��0x18(���Լ죬2000deg/s)
#define ACCEL_CONFIG		0x1C	//���ٶ�����-28 ���������Լ�������̷�Χ����ͨ�˲�Ƶ��
										//����ֵ��0x01(���Լ죬2G��5Hz)
#define ACCEL_XOUT_H	0x3B //59-65,���ٶȼƲ���ֵ XOUT_H
#define ACCEL_XOUT_L	0x3C  // XOUT_L
#define ACCEL_YOUT_H	0x3D  //YOUT_H
#define ACCEL_YOUT_L	0x3E  //YOUT_L
#define ACCEL_ZOUT_H	0x3F  //ZOUT_H
#define ACCEL_ZOUT_L	0x40 //ZOUT_L---64
#define TEMP_OUT_H		0x41 //�¶Ȳ���ֵ--65
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43 //������ֵ--67������Ƶ�ʣ��ɼĴ��� 25 ���壩д�뵽��Щ�Ĵ���
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48 //������ֵ--72
#define PWR_MGMT_1		0x6B //��Դ���� ����ֵ��0x00(��������)






//���һ��ȫ�ֵ��豸����
struct mpu_sensor{
	int dev_major;
	struct device *dev;
	struct class *cls;
	struct i2c_client *client;//��¼probe��client
};

struct mpu_sensor *mpu_dev;


int mpu6050_write_bytes(struct i2c_client *client, char *buf, int count)
{

	int ret;
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = 0;
	msg.len = count;
	msg.buf = buf;
	

	ret = i2c_transfer(adapter, &msg,  1);

	return ret==1?count:ret;

}

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

//��ȡĳ���ض��Ĵ����ĵ�ַ��Ȼ�󷵻�ֵ
int mpu6050_read_reg_byte(struct i2c_client *client, char reg)
{
	// ��д�Ĵ����ĵ�ַ�� Ȼ���ڶ��Ĵ�����ֵ

		int ret;
		struct i2c_adapter *adapter = client->adapter;
		struct i2c_msg msg[2];

		char rxbuf[1];
	
		msg[0].addr = client->addr;
		msg[0].flags = 0;
		msg[0].len = 1;
		msg[0].buf = &reg;

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
			//������
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

	//�����е����ݽ����û�
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
		�����豸�ţ�ʵ��fops
			�����豸�ļ�
			ͨ��i2c�Ľӿ�ȥ��ʼ��i2c���豸
	*/

	mpu_dev = kzalloc(sizeof(struct mpu_sensor), GFP_KERNEL);
	
	mpu_dev->client = client;

	mpu_dev->dev_major = register_chrdev(0,"mpu_drv", &mpu6050_fops);

	mpu_dev->cls = class_create(THIS_MODULE, "mpu_cls");

	mpu_dev->dev = device_create(mpu_dev->cls, NULL, MKDEV(mpu_dev->dev_major, 0),
				NULL, "mpu_sensor");
	
	char buf1[2] = {PWR_MGMT_1, 0x0};
	mpu6050_write_bytes(mpu_dev->client, buf1, 2);

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
			.compatible = "invensense,mpu6050",
		},
		{/*northing to be done*/},

};


const struct i2c_device_id mpu_id_table[] = {
		{"mpu6050_drv", 0x1111},
		{/*northing to be done*/},
};
	
struct i2c_driver mpu6050_drv = {
	.probe = mpu6050_drv_probe,
	.remove = mpu5060_drv_remove,
	.driver = {
		.name = "mpu6050_drv",//���д��/sys/bus/i2c/driver/mpu6050_drv
		.of_match_table = of_match_ptr(of_mpu6050_id),
	},
	
	.id_table = mpu_id_table,//���豸������µ�ƥ��,���豸����ģʽ�²���Ҫʹ��

};


static int __init mpu6050_drv_init(void)
{
	// 1������i2c driver����ע�ᵽi2c����
	return i2c_add_driver(&mpu6050_drv);

}

static void __exit mpu6050_drv_exit(void)
{
	i2c_del_driver(&mpu6050_drv);

}

module_init(mpu6050_drv_init);
module_exit(mpu6050_drv_exit);
MODULE_LICENSE("GPL");
