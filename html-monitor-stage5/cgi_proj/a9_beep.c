#include <stdio.h> 
#include "cgic.h" 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define N 8

struct msg
{
	long type;
	long msgtype;
	unsigned char text[N];
};


int cgiMain() 
{ 
	key_t key;
	char buf[N];
	char sto_no[2];
	int msgid;
	struct msg msg_buf;
	memset(&msg_buf,0,sizeof(msg_buf));
	
	cgiFormString("beep",buf,N);
	cgiFormString("store",sto_no,2);

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
	/*
	if (buf[0] == '1')
	{
		msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (1 << 0);
	}
	else
	{
		msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (0 << 0);
	}
	*/
	switch (buf[0])
	{
		case '0':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (0 << 0);
				break;
			}
		case '1':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (1 << 0);
				break;
			}
		case '2':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (2 << 0);
				break;
			}
		case '3':
			{
				msg_buf.text[0] = ((sto_no[0] - 48)) << 6 | (1 << 4) | (3 << 0);
				break;
			}
	}

	msg_buf.type = 1L;
	msg_buf.msgtype = 2L;
	msgsnd(msgid, &msg_buf,sizeof(msg_buf)-sizeof(long),0);

	sto_no[0] -= 48;

	cgiHeaderContentType("text/html\n\n"); 
	fprintf(cgiOut, "<HTML><HEAD>\n"); 
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n"); 
	fprintf(cgiOut, "<BODY>"); 

	fprintf(cgiOut, "<H2>send sucess</H2>");

	//fprintf(cgiOut, "<a href='.html'>返回</a>"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../a9_zigbee%d.html\">", sto_no[0]);
	fprintf(cgiOut, "</BODY>\n"); 
	fprintf(cgiOut, "</HTML>\n"); 


	return 0; 
} 
