#ifndef __BUZZER_H_
#define __BUZZER_H_


#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>


extern pthread_mutex_t  	mutex_buzzer;
extern pthread_cond_t      	cond_buzzer;

typedef struct beep_desc{
	int beep;    //2 3 4 5
	int beep_state;  //0 or 1
	int tcnt;  //占空比
	int tcmp;  //调节占空比
}beep_desc_t;

#define mytype 'f'
#define BEEP_ON 	_IOW(mytype,0,beep_desc_t)
#define BEEP_OFF 	_IOW(mytype,1,beep_desc_t)
#define BEEP_FREQ 	_IOW(mytype,2,beep_desc_t)



#endif

