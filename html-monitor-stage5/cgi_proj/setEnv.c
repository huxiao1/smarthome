//设置最大和最小阈值
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"
#include "data_global.h"
 
 
 
#define home_id 1
 
 struct shm_addr
 {
    char shm_status;   //shm_status可以等于home_id，用来区分共享内存数据
    struct env_info_client_addr  sm_all_env_info;
};
struct shm_addr *shm_buf;

#if 0
struct setEnv
{
	int temMAX;
	int temMIN;
	int humMAX;
	int humMIN;
	int illMAX;
	int illMIN;
};
#endif 

int cgiMain()
{
	key_t  key;
	char sto_no[2];
	char buf[20];
	struct shm_addr new;
	int msgid;
	struct msg msg_buf;
	
	memset(&msg_buf,0,sizeof(msg_buf));
	cgiFormString("store", sto_no, 2);

	cgiFormString("temMAX", buf, 4);
	new.sm_all_env_info.monitor_no[home_id].zigbee_info.tempMAX = atoi(buf);
	cgiFormString("temMIN", buf, 4);
	new.sm_all_env_info.monitor_no[home_id].zigbee_info.tempMIN = atoi(buf);
	cgiFormString("humMAX", buf, 4);
	new.sm_all_env_info.monitor_no[home_id].zigbee_info.humidityMAX = atoi(buf);
	cgiFormString("humMIN", buf, 4);
	new.sm_all_env_info.monitor_no[home_id].zigbee_info.humidityMIN = atoi(buf);
	
	//cgiFormString("illMAX", buf, 20);
	//new.illMAX = atoi (buf);
	//cgiFormString("illMIN", buf, 20);
	//new.illMIN = atoi (buf);

	if((key = ftok("/tmp", 'g')) < 0)
	{
		perror("ftok");
		exit(1);
	}

	if((msgid = msgget(key, 0666)) < 0)
	{
		perror("msgget");
		exit(1);
	}
	
	memcpy (msg_buf.text+1, &new, 24);
	
	msg_buf.type = 1L;
	msg_buf.msgtype = 5L;
	msg_buf.text[0] = sto_no[0];
	
	msgsnd(msgid, &msg_buf,sizeof(msg_buf)-sizeof(long),0);

	sto_no[0] -= 48;
	
	cgiHeaderContentType("text/html\n\n"); 
	fprintf(cgiOut, "<HTML><HEAD>\n"); 
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n"); 
	fprintf(cgiOut, "<BODY>"); 

	fprintf(cgiOut, "<H2>send sucess</H2>");

	//fprintf(cgiOut, "<a href='.html'>返回</a>"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../home%d.html\">", sto_no[0]);
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n"); 


	return 0; 

}
