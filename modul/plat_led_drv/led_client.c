#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>


#define SERVER_PORT 5001
//开发板ip地址
#define SERVER_IP "192.168.1.107"


int main(int argc,char **argv)
{

	int fd;
	int value;

	struct sockaddr_in ipv4addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd == -1){
		printf("client socket error\n");
		exit(1);
	}

	bzero(&ipv4addr,sizeof(ipv4addr));
	ipv4addr.sin_family = AF_INET;
	ipv4addr.sin_port = htons(SERVER_PORT);

	if(inet_pton(AF_INET,SERVER_IP,(void *)&ipv4addr.sin_addr.s_addr) != 1){
		perror("client inet_pton");
		exit(1);
	}

	printf("client starting...\n");

 	if(connect(fd,(struct sockaddr *)&ipv4addr,sizeof(ipv4addr)) < 0){
		perror("connect");

		exit(1);

	}

	while(1){
		//1代表点亮led，0代表熄灭led，其他数字代表客户端退出程序
		printf("please select to led on/off(1:on/0:off/other:quit)");

		scanf("%d",&value);

		if(value == 1 || value == 0)

			write(fd,&value,4);

		else{
			write(fd,&value,4);

			printf("client quiting...\n");

			break;
		}


		//吸收垃圾字符
		getchar();
	}

	close(fd);
	return 0;
}




 

