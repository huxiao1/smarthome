#include <stdio.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  

int main(int argc,char *argv[])  
{  
	int fd,len;  
	int adc;  
	fd = open("/dev/fsadc0",O_RDWR);  
	if(fd == -1){  
		perror("open fail \n");  
		return -1;  
	}  

	while(1)  
	{  
		read(fd,&adc,4);  
		sleep(1);
		printf("adc value :%0.2fV.\n",(1.8*adc)/4096);  
	}  

	close(fd);  
}  

#if 0

[root@makeru drv]# insmod adc.ko 
[  359.120000] --->fs4412_adc_init--->151.
[  359.120000] --->fs4412_adc_probe--->94.
[  359.125000] res2->start,res2->end - res2->start :0x1f.
[  359.130000] res1->start :168.
[  359.135000] major :252.


[root@makeru drv]# ./test 
---->fs4412_adc_open--->56
[  681.035000] fs4412_adc_open
[  681.040000] --->fs4412_adc_handler--->48.
adc value :1.31V.[  686.060000] --->fs4412_adc_handler--->48.
adc value :1.31V.[  687.065000] --->fs4412_adc_handler--->48.
adc value :1.31V.[  688.070000] --->fs4412_adc_handler--->48.
adc value :1.12V.[  689.075000] --->fs4412_adc_handler--->48.
adc value :1.12V.[  690.080000] --->fs4412_adc_handler--->48.
adc value :1.04V.[  691.080000] --->fs4412_adc_handler--->48.
adc value :0.96V.[  692.085000] --->fs4412_adc_handler--->48.
adc value :1.33V.[  694.095000] --->fs4412_adc_handler--->48.
adc value :1.44V.[  695.100000] --->fs4412_adc_handler--->48.
adc value :1.80V.[  696.105000] --->fs4412_adc_handler--->48.
adc value :1.80V.[  697.105000] --->fs4412_adc_handler--->48.
adc value :1.51V.[  698.110000] --->fs4412_adc_handler--->48.
adc value :0.38V.[  699.115000] --->fs4412_adc_handler--->48.
adc value :0.00V.[  700.120000] --->fs4412_adc_handler--->48.
adc value :0.00V.[  701.125000] --->fs4412_adc_handler--->48.

^C[  701.480000] ---->fs4412_adc_release--->81
[  701.480000] fs4412_adc_release


#endif 

