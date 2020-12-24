#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	//调用驱动
	int fd;
	int value = 0;

	fd = open("/dev/chr2", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	read(fd, &value, 4);

	printf("___USER___:  value = %d\n", value);

	//应用程序去控制灯的亮和灭
	while(1)
	{
		value = 0;
		write(fd, &value, 4);
		sleep(1);

		value = 1;
		write(fd, &value, 4);
		sleep(1);
		
	}


	close(fd);

	return 0;

}



