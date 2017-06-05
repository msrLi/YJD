/***********************************************************************
文件名称：TCP_CLient.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#include "main.h"
/***************开发板ip定义*************************/

char *BOARD_IP  	= "192.168.1.204";   		//开发板ip 
char *BOARD_NETMASK = "255.255.255.0";   		//开发板子网掩码
char *BOARD_WG		= "192.168.1.1";   		    //开发板子网关

#define TCP_SERVER_PORT			16882			//即TCP服务器端口号（远端）
//#define TCP_SERVER_IP			"119.90.35.78"	//即TCP服务器IP（远端）
//#define TCP_SERVER_IP			"222.35.226.137"	//即TCP服务器IP（远端）
#define TCP_SERVER_IP			"106.3.37.102"	//
//#define TCP_SERVER_IP			"192.168.1.108"	//

int client_sock_fd = -1;

char tcp_recv_flag = 0;
OS_EVENT  *sem_tcp_rec_flag = 0;				//tcp接收完一桢数据信号量定义

char tcp_recv_buff[BUF_SIZE];
int tcp_recv_len = 0;

extern int mcuID[3];

extern int TCP_Send_Data(int s,char *data,int len,int flags);

/***********************************************************************
函数名称：void ZQWL_W8782_IRQ_Handler(int socket, uint8_t *buff, int size)
功    能：网络数据处理函数，当有网络数据时，该函数被自动调用，网络的所有数据均在这里处理
输入参数：s为socket号，buff数据指针，size是数据长度 remote_addr 远程地址信息
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void ZQWL_W8782_IRQ_Handler(int s, uint8_t *buff, int size,struct sockaddr remote_addr)
{

	struct lwip_sock *sock;
	sock = get_socket(s);
	
	if(s == client_sock_fd)
	{
		if (sock -> conn -> type == NETCONN_TCP)//是TCP协议
		{
			memcpy(tcp_recv_buff,buff,size);//复制数据
			tcp_recv_flag = 1;
			tcp_recv_len = size;
			// TCP_Send_Data(client_sock_fd,tcp_recv_buff,tcp_recv_len,MSG_DONTWAIT);//原样返回数据
		}
	}
}

int send_ex(int sock, void * buffer, unsigned long len)
{
	int sendlen = 0, total_len = (int)len;

	
	while (total_len > 0)
	{
		sendlen = TCP_Send_Data(sock, buffer, total_len,MSG_DONTWAIT);//TCP客户端主动向服务器发送数据
		if (YJ_FAILURE == sendlen)
		{
			return YJ_FAILURE;
		}

		total_len -= sendlen;

		buffer = (void *)((unsigned long)buffer + sendlen);
	}

	return YJ_SUCCESS;
}

/***********************************************************************
函数名称：void Task_TCP_Client(void *pdata)
功    能：TCP客户端的任务，每隔500ms向服务器主动发送一次数据，并创建一个检测连接状态的任务，
		  该任务可以尽快得知服务器端是否断开连接
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_TCP_Client(void *pdata)
{
	struct lwip_sock *sock;
	INT8U 			err;
	parallet_msg_t	*pblock = NULL;
	OS_Q_DATA  		qInfo;
	int				i;
	INT8U	u8Buf[26];
	INT16U	len;
	INT16U  send_len;
	INT8U *send_buffer;
	int 	ret;
	static int wifi_time=0;
	
	//client_sock_fd = TCP_Link(TCP_SERVER_IP,TCP_SERVER_PORT);//向服务器发起连接
	//while(client_sock_fd == -1)//一直连接 直到成功
	//{
	//	printf("TCP Linking...\n");  //
	//	client_sock_fd = TCP_Link(TCP_SERVER_IP,TCP_SERVER_PORT);//向服务器发起连接
	//	OSTimeDlyHMSM(0, 0, 0, 500);//
	//}	
	//printf("TCP_Link ok!\n");  //	
	//创建一个任务用来检测client_sock_fd的状态
	//thread_create(Task_Client_Sock_Stats, 0, TASK_CLIENT_SOCK_STATS_PRIO, 0, TASK_CLIENT_SOCK_STATS_STACK_SIZE, "Task_Client_Sock_Stats");
	while(1)
	{
		err = OSQQuery(hParalletMsgQueue, &qInfo);
		if (OS_NO_ERR == err)
		{
			if (qInfo.OSNMsgs > 0)
			{
				pblock = (parallet_msg_t *)OSQAccept(hParalletMsgQueue, NULL);
				if (pblock != NULL)
				{
					send_len = 600;
					len = pblock->len;
					u8Buf[0]='$';
					u8Buf[1]='B';
					u8Buf[2]='Y';
					u8Buf[3]='P';
					u8Buf[4]='$';
					u8Buf[5]=0x01;
					u8Buf[6]=0x0C;
					u8Buf[7]=0xE9;
					u8Buf[8]=0x00;
					u8Buf[9]=0x00;
					*((YJ_U32*)(&u8Buf[10])) = ntohl(len+12);
					*((YJ_U32*)(&u8Buf[14]))=ntohl(mcuID[0]);
					*((YJ_U32*)(&u8Buf[18]))=ntohl(mcuID[1]);
					*((YJ_U32*)(&u8Buf[22]))=ntohl(mcuID[2]);
		 
					client_sock_fd = TCP_Link(TCP_SERVER_IP,TCP_SERVER_PORT);//向服务器发起连接
					wifi_time=0;
					while(client_sock_fd == -1)//一直连接 直到成功
					{
						printf("TCP Linking...\n");  //
						client_sock_fd = TCP_Link(TCP_SERVER_IP,TCP_SERVER_PORT);//向服务器发起连接
						OSTimeDlyHMSM(0, 0, 0, 500);//
						wifi_time++;
						if(wifi_time >= 60*5)
						{
							NVIC_SystemReset();
							/*
							ret = Init_W8782();				//初始化WIFI芯片
							Power_Mode(OFF_POWER_SAVE_MODE);	//不 进入省电模式，OFF_POWER_SAVE_MODE 为退出省电模式
							Init_Lwip(); 					//初始化lwip协议栈，并初始化了板子的IP
							ret = Wifi_Connect("yixiaoke","57018092");
							OSTimeDlyHMSM(0, 0, 1, 500);
							*/
						}
					}
					sock = get_socket(client_sock_fd);
					while(sock == NULL)
					{
						sock = get_socket(client_sock_fd);					
						printf("get sock ...\n");
						OSTimeDlyHMSM(0, 0, 0, 500);//

					}
					if(sock->conn->pcb.tcp->state == ESTABLISHED)//已经有连接
					{
						/* 发送头文件 */
						ret = send_ex(client_sock_fd, u8Buf,26);
						send_buffer = pblock->u8Buf;
						// 循环发送
						send_len = 600;
						do{
							if(send_len >len)
							{
								send_len = len;				
							}
							ret = send_ex(client_sock_fd, send_buffer,send_len);
							send_buffer += send_len;
							len -=  send_len;
							/*  发送失败 退出 */
							if(ret == YJ_FAILURE){
								printf("tcp send the date is error \n");
								break;
							}
						}while(len != 0);
						OSMemPut(g_hParalletRxMem, pblock);
						if (YJ_FAILURE == ret)
						{
							printf("???\n");
							NVIC_SystemReset();
						}
						TCP_Unlink(client_sock_fd);
					}
				}
			}
		}
		OSTimeDly(1);
	}
}

/***********************************************************************
函数名称：void Task_Client_Sock_Stats(void *pdata)
功    能：监视client_sock_fd连接的状态，如果断线重新连接，每秒检查一次
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_Client_Sock_Stats(void *pdata)
{
	struct lwip_sock *sock;
	while(1)
	{
		sock = get_socket(client_sock_fd);
		if(sock->conn->pcb.tcp->state != ESTABLISHED)//还没有连接
		{
			printf("TCP Link faild!\n");  //
			printf("TCP Link...!\n");  //
			client_sock_fd = TCP_Link(TCP_SERVER_IP,TCP_SERVER_PORT);//向服务器发起连接
			if(client_sock_fd != -1)
			{
				printf("TCP Link ok!\n");  //
			}
		}
		OSTimeDlyHMSM(0, 0, 1, 0);//挂起1s
	}
}

