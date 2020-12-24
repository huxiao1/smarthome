#ifndef __COMMON_H_
#define __COMMON_H_

struct mpu6050_data{
//union mpu6050_data{
	struct{
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}gyro;
	struct{
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}accel;
	unsigned short temp;
};

#define mytype 'F'

#define  MPU6050_ACCEL  _IOR(mytype,0, struct mpu6050_data)
#define  MPU6050_GYRO   _IOR(mytype,1, struct mpu6050_data)
#define  MPU6050_TEMP   _IOR(mytype,3, struct mpu6050_data)

#endif 



