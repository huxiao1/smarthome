
#include "data_global.h"
#include "common.h"
#include "mpu6050.h"
#include <strings.h>

//接收ZigBee的数据和采集的A9平台的传感器数据
int adc_fd;
int mpu_fd;

extern pthread_cond_t cond_transfer;
extern pthread_mutex_t mutex_transfer;
extern struct env_info_client_addr  sm_all_env_info;

int file_env_info_a9_zigbee_debug(struct env_info_client_addr *rt_status,int home_id);
int file_env_info_a9_zigbee_stm32(struct env_info_client_addr *rt_status,int home_id);

int printf_sensor_info_debug(struct env_info_client_addr  *sm_all_env_info,int home_id);

void *pthread_transfer(void *arg)
{
	int home_id = 1;
	adc_fd = open(ADC_DEV,O_RDWR);
	mpu_fd = open(MPU6050_DEV,O_RDWR);
	if((adc_fd == -1) || (mpu_fd == -1)){
		printf("open adc or mpu device failed.\n");
	}
	while(1){
		pthread_mutex_lock(&mutex_transfer);
		pthread_cond_wait(&cond_transfer,&mutex_transfer);
		//printf("pthread_analysis and tranfer.\n");
#if 1
		file_env_info_a9_zigbee_stm32(&sm_all_env_info,home_id);
#else	
		file_env_info_a9_zigbee_debug(&sm_all_env_info,home_id);
#endif
		pthread_mutex_unlock(&mutex_transfer);
		sleep(1);
	}
	close(adc_fd);
	close(mpu_fd);
}



//真实数据上传线程测试用的模拟数据
int file_env_info_a9_zigbee_debug(struct env_info_client_addr *rt_status,int home_id)
{
	static int temp = 0;
	int  env_info_size = sizeof(struct env_info_client_addr);
	//printf("env_info_size = %d.\n",env_info_size);
	
	rt_status->monitor_no[home_id].zigbee_info.temperature = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;

	temp ++;
	rt_status->monitor_no[home_id].a9_info.adc  = temp;
	rt_status->monitor_no[home_id].a9_info.gyrox  = -14.0;
	rt_status->monitor_no[home_id].a9_info.gyroy  = 20.0;
	rt_status->monitor_no[home_id].a9_info.gyroz  = 40.0;
	rt_status->monitor_no[home_id].a9_info.aacx  = 642.0;
	rt_status->monitor_no[home_id].a9_info.aacy  = -34.0;
	rt_status->monitor_no[home_id].a9_info.aacz  = 5002.0;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;
		
	//printf_sensor_info_debug(rt_status,home_id);

	//添加stm32部分的数据、arduino数据，
	
	return 0;
}


#if 0
int	get_sensor_data_from_a9(struct makeru_a9_info* a9_sensor_data)
{
	int adc_sensor_data;
	struct mpu6050_data data;

	/*get adc sensor data*/
	read(adc_fd,&adc_sensor_data,4); 
	printf("adc value :%0.2fV.\n",(1.8*adc_sensor_data)/4096);  

	/* get mpu6050 sensor data*/
	ioctl(mpu_fd,MPU6050_GYRO,&data);
	printf("gyro data: x = %05d, y = %05d, z = %05d\n", data.gyro.x,data.gyro.y,data.gyro.z);
	ioctl(mpu_fd,MPU6050_ACCEL,&data);
	printf("accel data: x = %05d, y = %05d, z = %05d\n", data.accel.x,data.accel.y,data.accel.z);

	/*预填充，有点浪费空间，大家可以优化一下*/
	a9_sensor_data->adc = (1.8 * adc_sensor_data)/4096 * 100; //定义未int32,应该是float,放大100倍，保护小数位
	a9_sensor_data->gyrox = data.gyro.x;
	a9_sensor_data->gyroy = data.gyro.y;
	a9_sensor_data->gyroz = data.gyro.z;
	a9_sensor_data->aacx  = data.accel.x;
	a9_sensor_data->aacy  = data.accel.y;
	a9_sensor_data->aacz  = data.accel.z;

	return 0;
}
#endif 

//真实数据上传
int file_env_info_a9_zigbee_stm32(struct env_info_client_addr *rt_status,int home_id)
{
	int  env_info_size = sizeof(struct env_info_client_addr);
	static int temp = 0;
	//printf("env_info_size = %d.\n",env_info_size);

	rt_status->monitor_no[home_id].zigbee_info.head[0]  = 'm';
	rt_status->monitor_no[home_id].zigbee_info.head[1]  = 's';
	rt_status->monitor_no[home_id].zigbee_info.head[2]  = 'm';
	rt_status->monitor_no[home_id].zigbee_info.head[3]  = 'z';
	rt_status->monitor_no[home_id].zigbee_info.temperature = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;

	//获取数据     
	int adc_sensor_data;
	struct mpu6050_data data;
	/*get adc sensor data*/
	//read(adc_fd,&adc_sensor_data,4);
	//printf("adc value :%0.2fV.\n",(1.8*adc_sensor_data)/4096); 
	temp++;
	rt_status->monitor_no[home_id].a9_info.adc    = temp;
	
	/* get mpu6050 sensor data*/
	ioctl(mpu_fd,MPU6050_GYRO,&data);
	//printf("gyro data: x = %d, y = %d, z = %d\n", data.gyro.x,data.gyro.y,data.gyro.z);
	ioctl(mpu_fd,MPU6050_ACCEL,&data);
	//printf("accel data: x = %d, y = %d, z = %d\n", data.accel.x,data.accel.y,data.accel.z);
	
	rt_status->monitor_no[home_id].a9_info.head[0]  = 'm';
	rt_status->monitor_no[home_id].a9_info.head[1]  = 's';
	rt_status->monitor_no[home_id].a9_info.head[2]  = 'm';
	rt_status->monitor_no[home_id].a9_info.head[3]  = 'a';

	rt_status->monitor_no[home_id].a9_info.gyrox  =  (short)data.gyro.x;
	rt_status->monitor_no[home_id].a9_info.gyroy  =  (short)data.gyro.y;
	rt_status->monitor_no[home_id].a9_info.gyroz  =  (short)data.gyro.z;

	rt_status->monitor_no[home_id].a9_info.aacx   =  (short)data.accel.x;
	rt_status->monitor_no[home_id].a9_info.aacy   =  (short)data.accel.y;
	rt_status->monitor_no[home_id].a9_info.aacz   =  (short)data.accel.z;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;

	//printf_sensor_info_debug(rt_status,home_id);

	return 0;
}

//测试打印数据用
int printf_sensor_info_debug(struct env_info_client_addr  *sm_all_env_info,int home_id)
{
	printf("a9_info.adc  : %f.\n",sm_all_env_info->monitor_no[home_id].a9_info.adc  );
	printf("a9_info.gyrox: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyrox);
	printf("a9_info.gyroy: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyroy);
	printf("a9_info.gyroz: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyroz);
	printf("a9_info.aacx : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacx );
	printf("a9_info.aacy : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacy );
	printf("a9_info.aacz : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacz );
	printf("a9_info.reserved[0]: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.reserved[0] );
	printf("a9_info.reserved[1]: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.reserved[1] );

	return 0;
}

