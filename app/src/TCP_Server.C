/***********************************************************************
文件名称：TCP_Server.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#include "main.h"
/***************开发板ip定义*************************/

char *BOARD_IP  	= "192.168.5.200";   		//开发板ip 
char *BOARD_NETMASK = "255.255.255.0";   		//开发板子网掩码
char *BOARD_WG		= "192.168.5.1";   		    //开发板子网关

#define TCP_LOCAL_PORT			1030			//即TCP服务器端口号

int tcp_server_sock_fd = -1;					//TCP服务器socket句柄
int a_new_client_sock_fd = -1;					//TCP客户端socket句柄

char tcp_recv_flag = 0;
OS_EVENT  *sem_tcp_rec_flag = 0;				//tcp接收完一桢数据信号量定义

char tcp_recv_buff[2048];						//tcp数据接收缓冲器
int tcp_recv_len = 0;							//tcp数据接收长度


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
	if(sock->conn->pcb.tcp->local_port == TCP_LOCAL_PORT)//与开发板TCP端口号一致
	{
		if (sock->conn->type == NETCONN_TCP)//是TCP协议
		{
			memcpy(tcp_recv_buff,buff,size);//复制数据
			tcp_recv_flag = 1;
			tcp_recv_len = size;
			a_new_client_sock_fd = s;
			OSSemPost(sem_tcp_rec_flag);  //抛出一个信号量表示tcp已经接收完成一帧数据
		}
	}
}
/***********************************************************************
函数名称：void Task_TCP_server(void *pdata)
功    能：TCP服务器的任务，在此可以实现具体功能，本例例程是将收到的数据原样返回。
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_TCP_server(void *pdata)
{
	unsigned  char  os_err;
	
	tcp_server_sock_fd = Create_TCP_Server(TCP_LOCAL_PORT);//初始化一个TCP服务socket 端口为TCP_LOCAL_PORT并开始监听
	sem_tcp_rec_flag = OSSemCreate(0); 						//创建一个信号量,
	while(1)
	{
		OSSemPend(sem_tcp_rec_flag,0,&os_err);				//持续等待sem_tcp_rec_flag信号量有效
		TCP_Send_Data(a_new_client_sock_fd,tcp_recv_buff,tcp_recv_len,MSG_DONTWAIT);//将数据原样返回
		LED_Ctrl(tcp_recv_buff);//增加一个简单的手机控制LED的实例
	}
}
