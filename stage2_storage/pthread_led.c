#include "data_global.h"
#include "chrdev.h"
#include <unistd.h>

extern pthread_mutex_t  	mutex_led;
extern pthread_cond_t      	cond_led;

int fswaterled_control(int led_fd, int times);
int fsled_control(int led_fd, unsigned char led_control_cmd);  //发送的数字
int fsled_close_all(int led_fd);

//:A9LED模块线程.
void *pthread_led(void *arg)
{
	printf("pthread_led\n");
	int i, j;
	int led_fd;
	led_desc_t led;

	led_fd = open(LED_DEV, O_RDWR);
	if(led_fd == -1){
		printf("open failed.\n");
	}
	printf("led_fd ;%d.\n",led_fd);

	while(1){
		pthread_mutex_lock(&mutex_led);
		pthread_cond_wait(&cond_led,&mutex_led);
		printf("led ioctl:***********\n");
		//	fswaterled_control(led_fd,2); //流水灯
		if(cmd_led == 0x41){
			fswaterled_control(led_fd,2); //流水灯
		}
		int tmp = cmd_seg & 0xf0;
		if(!(tmp ^ 0x70)){
			fsled_control(led_fd,cmd_seg);  //数码管	
		}
		pthread_mutex_unlock(&mutex_led);
	}

	close(led_fd);

}



int fsled_control(int led_fd, unsigned char led_control_cmd)
{
	int i = 0;
	led_desc_t led;
	led_control_cmd &= 0x0f;
	int shift_count = 1; //第0位，第1 - 3位 

	printf("led_control_cmd = %d.\n",led_control_cmd);
	fsled_close_all(led_fd);
	sleep(3);
	while(led_control_cmd){
		if(shift_count >= 5)
			break;
		if((led_control_cmd & 0x1) == 1){ //第0位开始 = LED2
			shift_count ++;  // = 2  LED2 
			printf("if shift_count :%d.\n",shift_count);
			led.which = shift_count; //led2 3 4 5 灯
			ioctl(led_fd,FSLEDON,&led);
			usleep(50000);  //让驱动响应的时间
		}else {
			shift_count ++;
			printf("else shift_count :%d.\n",shift_count);
			led.which = shift_count; //led2 3 4 5 灯
			ioctl(led_fd,FSLEDOFF,&led);
			usleep(50000);
		}
		led_control_cmd >>= 1;
	}

	return 0;
}


int fsled_close_all(int led_fd)
{
	int i = 0;
	led_desc_t led;	

	for(i = 2;i < 6;i ++){
		led.which = i;
		ioctl(led_fd,FSLEDOFF,&led);
		usleep(50000);
	}

	return 0;
}



int fswaterled_control(int led_fd, int times)
{
	int i = 0,j = 0;
	led_desc_t led;	

	for(j = 0;j < times;j ++){
		for(i = 2;i < 6;i ++){
			led.which = i;
			ioctl(led_fd,FSLEDON,&led);
			usleep(500000);

			led.which = i;
			ioctl(led_fd,FSLEDOFF,&led);
			usleep(500000);
		}
	}

	return 0;
}














