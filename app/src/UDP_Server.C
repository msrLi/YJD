/***********************************************************************
文件名称：UDP_Server.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#include "main.h"
/***************开发板ip定义*************************/

char *BOARD_IP  	= "192.168.1.200";   		//开发板ip 
char *BOARD_NETMASK = "255.255.255.0";   		//开发板子网掩码
char *BOARD_WG		= "192.168.1.1";   		    //开发板子网关

#define UDP_LOCAL_PORT			1030			//即UDP本地端口号

int server_sock_fd = -1;

OS_EVENT  *sem_udp_rec_flag = 0;				//udp接收完一桢数据信号量定义

char udp_recv_buff[2048];
int udp_recv_len = 0;

struct sockaddr udp_remote_client;

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
	if(s == server_sock_fd)
	{
		if (sock -> conn -> type == NETCONN_UDP)//是udp协议
		{
			memcpy(udp_recv_buff,buff,size);	//复制数据
			udp_recv_len = size;
			udp_remote_client = remote_addr;
			OSSemPost(sem_udp_rec_flag);  		//抛出一个信号量表示udp已经接收完成一帧数据
		}
	}
}
/***********************************************************************
函数名称：void Task_UDP_Server(void *pdata)
功    能：UDP服务器的任务，在此可以实现具体功能，本例例程是将收到的数据原样返回。
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_UDP_Server(void *pdata)
{
	unsigned  char  os_err;
	
	server_sock_fd = UDP_Create_Server(UDP_LOCAL_PORT);//创建UDP服务器
	if(server_sock_fd == -1)
	{
		printf("UDP_Create_Server faild!\n");
		while(1);
	}	
	printf("UDP_Create_Server ok!\n");  //	
	sem_udp_rec_flag = OSSemCreate(0); 						//创建一个信号量,
	while(1)
	{
		OSSemPend(sem_udp_rec_flag,0,&os_err);				//持续等待sem_udp_rec_flag信号量有效
		UDP_Sendto_Data(server_sock_fd,udp_recv_buff,udp_recv_len,udp_remote_client,MSG_DONTWAIT);//将数据原样返回
	}
}

