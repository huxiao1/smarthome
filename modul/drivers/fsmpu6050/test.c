/*************************************************************************
	#	 FileName	: test.c
	#	 Author		: fengjunhui 
	#	 Email		: 18883765905@163.com 
	#	 Created	: 2017年07月03日 星期一 15时48分02秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 #include <sys/ioctl.h>
#include "common.h"
#include "mpu6050.h"


void delay_ms(unsigned char times)
{
	int i = 0;
	for( ;times > 0;times --)
	{
		for(i = 1;i > 0;i -- ){
			usleep(1000);
		}
	}
}


int main(int argc, const char *argv[])
{

	int i;
	int fd;
	struct mpu6050_data data;

	char usrbuf[4]={0};
	char kbuf[4]={0};
	fd = open("/dev/fsmpu60500",O_RDWR);
	if(fd < 0){
		printf("open failed !\n");
		return -1;
	}
	printf("open device success! fd: %d\n",fd);
	
	read(fd,usrbuf,4);
	printf("usrbuf[2] : %c\n",usrbuf[2]);

	usrbuf[0] = '9';
	write(fd,usrbuf,4);
		

	while(1){

		printf("***********************************gyro*******************\n");
		ioctl(fd,MPU6050_GYRO,&data);
		printf("gyro data: x = %05d, y = %05d, z = %05d\n", data.gyro.x,data.gyro.y,data.gyro.z);
		sleep(1);	
		
		printf("----------------------------------accel-------------------\n");
		ioctl(fd,MPU6050_ACCEL,&data);
		printf("accel data: x = %05d, y = %05d, z = %05d\n", data.accel.x,data.accel.y,data.accel.z);
		sleep(1);
		
		printf("===================================temp===================\n");
		ioctl(fd,MPU6050_TEMP,&data);
		printf("temp data: z = %05d\n",data.temp);

		sleep(1);
	}

	close(fd);

	return 0;
}
