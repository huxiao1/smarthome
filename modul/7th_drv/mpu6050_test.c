#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#include "mpu6050.h"


int main(int argc, char *argv[])
{
	int fd;
	
	union mpu6050_data data;
	
	fd = open("/dev/mpu_sensor", O_RDWR);     //打开设备结点
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	while(1)
	{
		ioctl(fd, IOC_GET_ACCEL, &data);
		printf("accel data :  x = %d, y=%d, z=%d\n", data.accel.x, data.accel.y, data.accel.z);

	
		ioctl(fd, IOC_GET_GYRO, &data);
		printf("gyro data :  x = %d, y=%d, z=%d\n", data.gyro.x, data.gyro.y, data.gyro.z);

		sleep(1);
		sleep(1);
		sleep(1);

	}

	close(fd);
	

	return 0;

}
