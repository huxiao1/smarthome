/*************************************************************************
#	 FileName	: test.c
#	 Author		: fengjunhui 
#	 Email		: 18883765905@163.com 
#	 Created	: 2018年07月03日 星期二 15时26分08秒
 ************************************************************************/

#include<stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "chrdev.h"

int main(int argc, const char *argv[])
{
	int i = 0,j = 0;
	int fd;
	ssize_t rbytes,wbytes;
	char rbuf[4] = {0};
	led_desc_t led;

	//fd = open("/dev/demo0", O_RDWR,0777);
	fd = open("/dev/fsled0", O_RDWR,0777);
	if(fd == -1){
		printf("open failed .\n");
	}
	printf("fd :%d.\n",fd);

	rbytes = read(fd, rbuf,sizeof(rbuf)/sizeof(rbuf[0]));
	if(rbytes == -1){
		printf("read failed from kernel buf.\n");
	}
	printf("rbytes :%d.\trbuf[2] :%d.\n",rbytes,rbuf[2]);

	printf("----------------------%d.\n",__LINE__);
	rbuf[2] = 99;
	wbytes = write(fd,rbuf,4);
	if(wbytes == -1){
		printf("write failed to kernel buf.\n");
	}
	for(j = 0;j < 3;j ++){
		for(i = 2;i < 6;i ++){
			led.which = i;
			ioctl(fd,FSLEDON,&led);
			printf("-on---------------------%d.\n",__LINE__);
			usleep(500000);

			led.which = i;
			ioctl(fd,FSLEDOFF,&led);
			printf("-off---------------------%d.\n",__LINE__);
			usleep(500000);
		}
	}

#if 0
	led.state = 0;
	ioctl(fd,FSLEDOFF,&led);
	printf("-off---------------------%d.\n",__LINE__);

	ioctl(fd,FSLEDON,&led);
	printf("-on---------------------%d.\n",__LINE__);
#endif 

	printf("----------------------%d.\n",__LINE__);

	close(fd);
	return 0;
}
