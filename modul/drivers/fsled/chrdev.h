/*************************************************************************
	#	 FileName	: chrdev.h
	#	 Author		: fengjunhui 
	#	 Email		: 18883765905@163.com 
	#	 Created	: 2018年07月04日 星期三 10时32分55秒
 ************************************************************************/

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


