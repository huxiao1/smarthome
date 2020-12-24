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
	
	fd = open("/dev/event1", O_RDWR);
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
			switch(event.code){
				case KEY_POWER:
					if(event.value){ //按下
						printf("__APP_USER__ :  power pressed\n");

					}else{
						printf("__APP_USER__ :  power up\n");
					}
					break;
				case KEY_VOLUMEDOWN:
					if(event.value){ //按下
						printf("__APP_USER__ :  KEY_VOLUMEDOWN pressed\n");

					}else{
						printf("__APP_USER__ :  KEY_VOLUMEDOWN up\n");
					}
					break;
				case KEY_VOLUMEUP:
					if(event.value){ //按下
						printf("__APP_USER__ :  KEY_VOLUMEUP pressed\n");

					}else{
						printf("__APP_USER__ :  KEY_VOLUMEUP up\n");
					}
					break;
	
			}

		}
		
	}


	close(fd);

	return 0;
}








