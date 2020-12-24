#ifndef __MPU6050_H__
#define __MPU6050_H__

union mpu6050_data{
	struct{	
		short x;
		short y;
		short z;
	}accel;

	struct{	
		short x;
		short y;
		short z;
	}gyro;

	short temp;
};


#define IOC_GET_ACCEL  _IOR('M', 0x34,union mpu6050_data)
#define IOC_GET_GYRO  _IOR('M', 0x35,union mpu6050_data)
#define IOC_GET_TEMP  _IOR('M', 0x36,union mpu6050_data)


#endif

