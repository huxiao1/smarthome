#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>


#define KEY_ENTER 28

//设计一个描述按键数据的对象
struct key_event{
	int code;     //按键的类型
	int value;    //表示状态 按下还是抬起
};


int main(int argc, char *argv[])
{
	struct key_event event;
	//调用驱动
	int fd;
	int ret;
	char in_buf[128];

	fd = open("/dev/key3", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	//监控多个文件描述符
	struct pollfd pfd[2];
 
	pfd[0].fd = fd;        //监控按键设备
	pfd[0].events = POLLIN;

	pfd[1].fd = 0;      //标准输入
	pfd[1].events = POLLIN;

	while(1)
	{
		ret = poll(pfd, 2, -1);
		if(ret < 0)
			{
				perror("poll");
				exit(1);
			}
		else   //两个fd至少一个有数据
			{
				if(pfd[0].revents & POLLIN)
					{
						read(pfd[0].fd,&event,sizeof(struct key_event));
						if(event.code == KEY_ENTER)
						{
							if(event.value == 1)
							{
								printf("app_key enter pressed!\n");
							}
							else
							{
								printf("app_key enter up!\n");
							}
						}
					}
				if(pfd[1].revents & POLLIN)
					{
						fgets(in_buf,128,stdin);     //阻塞函数,但是加入了poll则不会阻塞
						printf("in_buf = %s\n", in_buf);
					}
			}
		}
	
	


	close(pfd[0].fd);

	return 0;

}



