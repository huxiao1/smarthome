# smarthome

两种意识：
	1、分层意识
	2、数据流

*****************************************************
分层分析： 
##################
web网页端显示部分： 
		环境信息 === 实时刷新环境数据
		摄像头采集图像  ===  采集监控信息
		硬件控制 === 下发要去控制的命令
A9数据处理部分
		创建进程、线程
		每条线程做自己的事情
		涉及到进程间通信
		数据处理===>分发（上行数据 or 下行数据)		
A9-ZigBee数据采集部分
		A9采集部分
		ZigBee采集部分
		(STM32平台（可以自己扩展）)
*****************************************************
数据流分析：
##################
		数据上传：
		数据下发：
					
	制定通信的协议（结构体）：
	数据要怎么上传，上传的目的是为了什么？
	数据要怎么下发，下发的目的又是为了什么？
	
		数据的上传： ====> 共享内存上传数据 ====> 显示并交给用户查看环境信息
		数据的下发用于控制硬件：====> 消息队列下发数据 ===> 控制硬件改变环境
	
**************************************************************************

分层分析： 
web网页端显示部分： 
		环境信息：  		adc电压数据
							mpu6050的六轴数据
							温度
							湿度
		摄像头采集图像：
							
		硬件控制：			风扇
							LED灯
							蜂鸣器
							GPRS ==== 发短信或打电话

A9数据处理部分
	数据流向分析：
		1、ZigBee(采集终端)-->A9（处理平台）
		2、A9（处理平台）-->网页（显示平台）
		3、网页（显示平台）-->A9（处理平台）
		4、A9（处理平台）--->ZigBee(采集终端)
	
A9-ZigBee采集部分
			外设驱动 --------在应用层去获取外设的状态或数据
			
A9--------- 蜂鸣器  ------------------蜂鸣器报警
			LED灯   ------------------卧室-厕所-楼道-公共照明 --------LED2-LED3-LED4-LED5
			按键    ------------------按键触发中断---控制卧室和厕所灯-----LED2-LED3 
			ADC      -----------------获取ADC的采样数据
			mpu6050 ------------------获取MPU6050的六轴数据

zigbee------adc     ------主---协调器
			风扇    ------从---终端节点   下发命令控制风扇
			温湿度	------从---终端节点   上传温湿度数据
			(光敏)
			
小结： 					 
		|	        |             |             |
		|	ZigBee  |   A9 		  |      web    |
		|	        |             |             |
        |   adc     |   蜂鸣器       |  环境信息：-----------------adc电压数据
        |   风扇     |   LED灯       |  摄像头采集:-----usb摄像头  mpu6050的六轴数据
        |   温湿度	|   按键            |  硬件控制：	|------风扇     温度
        | （光敏）	|   ADC           |   			|      LED灯    湿度
                        mpu6050                        蜂鸣器
                        四路led灯模拟数码管			   GPRS
													   四路led灯模拟数码管	
						
数据流分析：
	数据上传： 
		ZigBee                 | 
			温湿度数据         |
		A9                     |
			ADC采集            |-----------上传这些数据
			加速计数据         |
			陀螺仪数据         |
		摄像头                 |
			视频流图像         |
			
	数据下发：                    
		ZigBee：                      |
			风扇                      |
		A9：                          |
			蜂鸣器                    |-----打开设备节点控制硬件
			LED灯                     |
			四路LED灯模拟的数码管     |
		GPRS：                        |
			3G通信模块                |


		    #define		GPRS_DEV   		 "/dev/ttyUSB0"
		    #define		ZIGBEE_DEV 		 "/dev/ttyUSB1"
		    #define		BEEPER_DEV 		 "/dev/fsbeeper0"
		    #define		LED_DEV    		 "/dev/fsled0"
		
制定通信的结构体：

	数据的上传： 
	数据类型定义： 	
		typedef uint8_t  unsigned char;       =======参考：
		typedef uint16_t unsigned short;
		typedef uint32_t unsigned int;

		//考虑到内存对齐的问题
		struct makeru_zigbee_info{
				uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
				uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9
				------------->crc ...加密算法 <--------------
				float temperature; //温度
				float humidity;  //湿度
				float tempMIN;//温度下限
				float tempMAX;//温度上限 
				float humidityMIN;   //湿度下限
				float humidityMAX;   //湿度上限
				uint32_t reserved[2]; //保留扩展位，默认填充0
				//void *data;  内核预留的扩展接口  参考版
		};

		struct makeru_a9_info{
			uint8_t head[3]; //标识位: 'm' 's' 'm'  makeru-security-monitor  
			uint8_t type;	 //数据类型  'z'---zigbee  'a'---a9
			uint32_t adc；
			short gyrox;   //陀螺仪数据
			short gyroy;
			short gyroz;
			short  aacx;  //加速计数据
			short  aacy;
			short  aacz;
			uint32_t reserved[2]; //保留扩展位，默认填充0
			//void *data;  内核预留的扩展接口  参考版
		};
		
		struct makeru_env_data{
			struct makeru_a9_info       a9_info;    
			struct makeru_zigbee_info   zigbee_info;
		};
		
		//所有监控区域的信息结构体
		struct env_info_client_addr
		{
			struct makeru_env_data  monitor_no[MONITOR_NUM];	//数组  老家---新家
		};
		
		
		
	数据的下发：（采用消息队列的方式下发数据到下位机上） 
		数据的下发用于控制硬件： 
		
		man msgsnd

	
	   #include <sys/types.h>
       #include <sys/ipc.h>
       #include <sys/msg.h>

       int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
       ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
                      int msgflg);
		消息队列用于通信的结构体： 包括数据类型和数据

		
		将消息队列封装成函数，直接通过参数传递的方式来发送信息： 
		int send_msg_queue(long type,unsigned char text)
		{
			struct msg msgbuf;
			msgbuf.type = 1L;
			msgbuf.msgtype = type;   //具体的消息类型
			msgbuf.text[0] = text;   //控制命令字 
			if(msgsnd(msgid,&msgbuf,sizeof(msgbuf) - sizeof(long),0) == -1){
				perror("fail to msgsnd type2");
				exit(1);
			}
			return 0;
		}
		
		struct msgbuf {
           long mtype;       /* message type, must be > 0 */
           char mtext[1];    /* message data */
        };

		//消息队列结构体
		#define QUEUE_MSG_LEN 32                 
		struct msg
		{
		    long type;   //从消息队列接收消息时用于判断的消息类型  ==== 暂时不用 1L===home1  2L===home2 ... 
		    
			long msgtype;//具体的消息类型 === 指代控制的设备，是什么类型的设备
		    unsigned char text[QUEUE_MSG_LEN];//消息正文  ====> CMD 控制指定的设备
		};
		
		long msgtype;//具体的消息类型
		消息类型的分配：
			1L: 		                LED控制
			2L:			蜂鸣器控制
			3L:			四路LED灯模拟的数码管
			4L:			风扇
			5L:			温湿度最值设置
			6L-7L-8L-9L,                             用于个人的扩展
			10L: 		                3 G通信模块-GPRS 
		switch(msgbuf.msgtype){
			case 1L: ...  break;
			....
			default ....  break;
		}
		
		
		控制命令的制定：
		
		消息队列接收消息：
			msgrcv (msgid, &msgbuf, sizeof (msgbuf) - sizeof (long), 1L, 0);
		解析buf中的数据：
			printf ("Get %ldL msg\n", msgbuf.msgtype);
			printf ("text[0] = %#x\n", msgbuf.text[0]);
		
		
		A9-ZIGBEE通用指令
		命令格式：一个字节，unsigned char 对应消息队列中正文的类型： 
							unsigned int 
		8位
		----------------------------------------
		7	6	|  5	4	|	3	2	1	0
		平台编号|  设备编号 |	操作设备
		----------------------------------------
		
		 0   0
		 0   1 
		 1   0
		 1   1
		 
		平台编号	
		0x00		0号-ZigBee平台 
		0x40		1号-A9/A53平台
		0x80		2号-STM32平台（可以自己扩展）
		0xc0		3号-avr arduino....保留(如果平台继续增多的话可以采用2个字节或多个字节来对设备进行
								唯一的编号，比如A9类下的1号平台，2号平台，先分类，然后再具体标识设备)
		
		----------------------------------------		
		设备编号		操作掩码	
		0x00	LED		0x00	全部关闭
						0x01	全部打开
						0x02	打开LED2
						0x03	打开LED3
						0X04	打开LED4
						0x05	打开LED5
						0X10	打开流水灯
		----------------------------------------
		0x10	蜂鸣器	0x00	关闭
						0x01	打开
						0x02	自动报警关闭
						0x03	自动报警打开
		----------------------------------------
		0x20	风扇	0x00	关闭风扇
						0x01	打开风扇
		----------------------------------------			
		0x30	数码管	0x0~0xF	    显示0~F数字(四盏灯，对应0000-表示0,0001-表示1....1110-表示14)
						0x0f		关闭数码管				led2-3-4-5
		----------------------------------------
		
		控制命令：		
			平台编号 + 设备编号 + 操作掩码 = 命令 (命令的封装)
			例如：
					0x00 + 0x20 + 0x01 = 0x21   风扇打开
 
					0x40 + 0x10 + 0x01 = 0x51   蜂鸣器打开
					0x40 + 0x30 + 0x08 = 0x78   数码管显示8
					0x40 + 0x30 + 0x0f = 0x7f   关闭数码管
					
					a 高位数据，b代表低位数据
					short  c
					unsigned char a ,b;
					c = a | b;
					c = a + b;
				
	上行：封装的结构体====共享内存和信号量 ===>交给CGI（C语言和HTML语言之间的转化接口）===>交给HTML
	下行：封装的命令字====消息队列 ====>msgbuf msgsnd===>控制命令字封装在msgsnd的msgbuf中 ===>A9端解析==>向下控制硬件







