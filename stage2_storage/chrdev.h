//用于led驱动线程

#ifndef __CHRDEV_H_
#define __CHRDEV_H_

typedef struct led_desc{
	int which;
	int state;
}led_desc_t;

#define mytype 'f'

#define FSLEDON    _IOW(mytype,1,led_desc_t)
#define FSLEDOFF   _IOW(mytype,0,led_desc_t)

#endif 


