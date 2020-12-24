



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main(void)
{
	int fd;
	int ret;
	struct input_event event;
	
	fd = open("/dev/input/event1", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}


	while(1)
	{
		ret = read(fd, &event, sizeof(struct input_event));
		if(ret < 0)
		{
			perror("read");
			exit(1);
		}

		if(event.type == EV_KEY){
			if(event.code == KEY_POWER){
				if(event.value){ //°´ÏÂ
					printf("__APP_USER__ :  power pressed\n");

				}else{
					printf("__APP_USER__ :  power up\n");
				}
			}

		}
		
	}


	close(fd);

	return 0;
}








