#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

#define KEY_ENTER 28

static int fd;
static struct key_event event;

//设计一个描述按键数据的对象
struct key_event{
	int code;     //按键的类型
	int value;    //表示状态 按下还是抬起
};

void catch_signale(int signo)
{
   if(signo == SIGIO)
   {
       	printf("we got sigal SIGIO:\n");
        // 读取数据
        read(fd, &event, sizeof(struct key_event));
        if(event.code == KEY_ENTER)
        {
             if(event.value)
             {
                  printf("APP__ key enter pressed\n");
              }else
              {
                  printf("APP__ key enter up\n");
              }
         }
   }
}



int main(int argc, char *argv[])
{
	
	//调用驱动
	
	int ret;
	char in_buf[128];

	fd = open("/dev/key3", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	//1.设置信号处理方法
	signal(SIGIO,catch_signale);
	//2.将当前进程设置成SIGIO的主属性
	fcntl(fd,F_SETOWN,getpid());
	//3.将io模式设置成异步模式
	int flags = fcntl(fd,F_GETFL);        //拿出
	fcntl(fd,F_SETFL,flags | FASYNC);     //写回


	while(1)
	{
		//可做其他事情
		printf("i am waiting------\n");
		sleep(1);
	}
	


	close(fd);

	return 0;

}



