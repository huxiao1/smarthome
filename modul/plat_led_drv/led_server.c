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

int main(int argc,char **argv)
{

	int fd;
	int newfd;
	int len;
	int flag = 1;

	//设备节点的pfd
	int pfd;
	int value;
	struct sockaddr_in ipv4addr;
	struct sockaddr_in clientaddr;

	char ipaddr[16];
	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd == -1){
		printf("server socket error\n");
		exit(1);
	}

	bzero(&ipv4addr,sizeof(ipv4addr));

	ipv4addr.sin_family = AF_INET;

	ipv4addr.sin_port = htons(SERVER_PORT);	

	ipv4addr.sin_addr.s_addr = htonl(INADDR_ANY);	

	if(bind(fd, (struct sockaddr*)&ipv4addr,sizeof(ipv4addr)) < 0){

		perror("bind");

		exit(1);
	}

	if(listen(fd,5) < 0){

		perror("listen");

		exit(1);
	}

	printf("server starting...\n");


	//地址复用
	if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int))==-1)
	{
    	perror("setsockopt"); 
      	exit(1); 
	}


	len = sizeof(clientaddr);
	newfd = accept(fd, (struct sockaddr*)&clientaddr,&len);


	if(newfd == -1)
	{
		perror("accept");

		exit(1);
	}


	//打印客户端的ip和port
	if(!inet_ntop(AF_INET,(void*)&clientaddr.sin_addr.s_addr,ipaddr,sizeof(clientaddr))){
		perror("inet_ntop");

		exit(1);
	}

	printf("client ip:%s,port:%d is connected\n",ipaddr,ntohs(clientaddr.sin_port));

	pfd = open("/dev/led0",O_RDWR);

	if(pfd < 0){

		perror("open");

		exit(1);

	}

	while(1){
		//读取客户端通过socket发来的数据
		read(newfd,&value,4);

		if(value == 1 || value ==0)
			//间接控制设备
			write(pfd,&value,4);

		else{
			printf("client quiting...\n");
			break;
		}
	}
	close(newfd);
	close(fd);
	close(pfd);


	return 0;
}
