#ifndef __CHARDEV_H_
#define __CHARDEV_H_

#define mytype 'f'

typedef struct led_desc{
	int led_num;    //2 3 4 5
	int led_state;  //0 or 1
}led_desc_t;

#define FS_LED_ON  _IOW(mytype,1,led_desc_t)
#define FS_LED_OFF _IOW(mytype,0,led_desc_t)

#endif

