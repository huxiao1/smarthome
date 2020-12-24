#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc,char *argv[])
{
	int fd;
	int on = 0;
	
	fd = open("/dev/led0",O_RDWR);
	if(fd < 0)
		{
			perror("open error\n");
			exit(1);
		}

	while(1)
		{
			on = 0;
			write(fd,&on,4);
			sleep(1);

			
			on = 1;
			write(fd,&on,4);
			sleep(1);

		}

	close(fd);
	return 0;

}







