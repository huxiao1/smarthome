#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "data_global.h"


void release_pthread_resource(int signo);

extern pthread_mutex_t mutex_client_request,      //互斥锁
        		mutex_refresh,
        		mutex_sqlite,
	        	mutex_transfer,
	        	mutex_sms,
	        	mutex_buzzer,
	         	mutex_led;

extern pthread_cond_t  cond_client_request,      //条件变量
        		cond_refresh,
        		cond_sqlite,
	        	cond_transfer,
	        	cond_transfer,
	        	cond_sms,
	        	cond_buzzer,
	         	cond_led;

extern int msgid;
extern int shmid;
extern int semid;

pthread_t  id_client_request,
		  id_refresh,
		  id_sqlite,
		  id_transfer,
		  id_sms,
		  id_buzzer,
		  id_led;		


int main(int argc, const char *argv[])
{
	//初始化锁
	pthread_mutex_init(&mutex_client_request,NULL);
	pthread_mutex_init(&mutex_refresh,NULL);
	pthread_mutex_init(&mutex_sqlite,NULL);
	pthread_mutex_init(&mutex_transfer,NULL);
	pthread_mutex_init(&mutex_sms,NULL);
	pthread_mutex_init(&mutex_buzzer,NULL);
	pthread_mutex_init(&mutex_led,NULL);
	
	//等待接受信号，信号处理函数
	signal (SIGINT, release_pthread_resource);

	//初始化各种条件变量
	pthread_cond_init(&cond_client_request,NULL);
	pthread_cond_init(&cond_refresh,NULL);
	pthread_cond_init(&cond_sqlite,NULL);
	pthread_cond_init(&cond_transfer,NULL);
	pthread_cond_init(&cond_sms,NULL);
	pthread_cond_init(&cond_buzzer,NULL);
	pthread_cond_init(&cond_led,NULL);


	pthread_create(&id_client_request,  NULL,pthread_client_request,NULL);   
	pthread_create(&id_refresh,		NULL,pthread_refresh,NULL);  			
	pthread_create(&id_sqlite,		NULL,pthread_sqlite,NULL);  			
	pthread_create(&id_transfer,	NULL,pthread_transfer,NULL); 
	pthread_create(&id_sms,			NULL,pthread_sms,NULL);  	
	pthread_create(&id_buzzer,		NULL,pthread_buzzer,NULL);	 
	pthread_create(&id_led,			NULL,pthread_led,NULL);  	

	//等待线程退出
	pthread_join(id_client_request,NULL);     printf ("pthread1\n");
	pthread_join(id_refresh,NULL);          	printf ("pthread2\n");
	pthread_join(id_sqlite,NULL);			printf ("pthread3\n");
	pthread_join(id_transfer,NULL);			printf ("pthread4\n");
	pthread_join(id_sms,NULL);				printf ("pthread5\n");
	pthread_join(id_buzzer,NULL);			printf ("pthread6\n");
	pthread_join(id_led,NULL);				printf ("pthread7\n");

	return 0;
}


void release_pthread_resource(int signo)
{
	//释放锁和条件变量
	pthread_mutex_destroy (&mutex_client_request);   
	pthread_mutex_destroy (&mutex_refresh);   
	pthread_mutex_destroy (&mutex_sqlite);    
	pthread_mutex_destroy (&mutex_transfer);   
	pthread_mutex_destroy (&mutex_sms);   
	pthread_mutex_destroy (&mutex_buzzer);   
	pthread_mutex_destroy (&mutex_led);   

 	pthread_cond_destroy (&cond_client_request);
 	pthread_cond_destroy (&cond_refresh);
 	pthread_cond_destroy (&cond_sqlite);
 	pthread_cond_destroy (&cond_transfer);
 	pthread_cond_destroy (&cond_sms);
 	pthread_cond_destroy (&cond_buzzer);
 	pthread_cond_destroy (&cond_led);

	//分离线程的资源
	 pthread_detach(id_client_request);
	 pthread_detach(id_refresh);
	 pthread_detach(id_sqlite);
	 pthread_detach(id_transfer);
	 pthread_detach(id_sms);
	 pthread_detach(id_buzzer);
	 pthread_detach(id_led);

	 printf("all pthread is detached\n");
	 
	 msgctl (msgid, IPC_RMID, NULL);    //msgctl系统调用对msqid标识的消息队列执行cmd操作列
	 shmctl (shmid, IPC_RMID, NULL);    //共享内存
	 semctl (semid, 1, IPC_RMID, NULL);  //信号量

	 exit(0);
}

/*“信号量用在多线程多任务同步的，一个线程完成了某一个动作就通过信号量告诉别的线程，
别的线程再进行某些动作（大家都在semtake的时候，就阻塞在 哪里）。而互斥锁是用在多线程多任务互斥的，
一个线程占用了某一个资源，那么别的线程就无法访问，直到这个线程unlock，其他的线程才开始可以利用这 
个资源。比如对全局变量的访问，有时要加锁，操作完了，在解锁。有的时候锁和信号量会同时使用的”
*/

