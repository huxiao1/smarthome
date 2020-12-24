#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

#define N 32

struct msg
{
	long type;
	long msgtype;
	char text[N];
};

struct From_to_send{
	char to_number[12];
	char center_number[12];
}phone_NUM;

void get_set_sms()
{
	cgiFormStringNoNewlines("to",phone_NUM.to_number,12);
	cgiFormStringNoNewlines("center",phone_NUM.center_number,12);
}

int cgiMain()
{
	key_t key;
	int msgid;
	char sto_no[2];	
	get_set_sms();
	struct msg msg_buf;
	memset(&msg_buf,0,sizeof(msg_buf));
	
	cgiFormString("store", sto_no, 2);

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
	strcat(phone_NUM.center_number,phone_NUM.to_number);
	strcpy(msg_buf.text,phone_NUM.center_number);
	

	msg_buf.type = 1L;
	msg_buf.msgtype = 10L;
	msgsnd(msgid, &msg_buf,sizeof(msg_buf)-sizeof(long),0);

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML><HEAD>\n"); 
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n"); 
	fprintf(cgiOut, "<BODY bgcolor=\"#fffffffff\">\n");
	fprintf(cgiOut, "<H2>setting send tel  message successfully!</H2>");
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../home%d.html\">",sto_no[0] - '0');
	fprintf(cgiOut, "</BODY></HTML>\n");
	return 0;
}
