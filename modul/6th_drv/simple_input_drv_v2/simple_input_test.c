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
				if(event.value){ //按下
					printf("__APP_USER__ :  power pressed\n");

				}else{
					printf("__APP_USER__ :  power up\n");
				}
			}
			if(event.code == KEY_VOLUMEDOWN){
				if(event.value){ //按下
					printf("__APP_USER__ :  VOLUMEDOWN pressed\n");

				}else{
					printf("__APP_USER__ :  VOLUMEDOWN up\n");
				}
			}
			if(event.code == KEY_VOLUMEUP{
				if(event.value){ //按下
					printf("__APP_USER__ :  VOLUMEUP pressed\n");

				}else{
					printf("__APP_USER__ :  VOLUMEUP up\n");
				}
			}

		}
		
	}


	close(fd);

	return 0;
}








