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

#define UDP_LOCAL_PORT			5000			//即UDP本地端口号
#define UDP_REMOTE_PORT			1030			//即UDP远端端口号
#define	UDP_REMOTE_IP			"192.168.1.107"	//远端主机IP

int client_sock_fd = -1;
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
	if(s == client_sock_fd)
	{
		if (sock -> conn -> type == NETCONN_UDP)//是udp协议
		{
			memcpy(udp_recv_buff,buff,size);	//复制数据
			udp_recv_len = size;
			udp_remote_client = remote_addr;
			UDP_Sendto_Data(client_sock_fd,udp_recv_buff,udp_recv_len,udp_remote_client,MSG_DONTWAIT);//将数据原样返回
		}
	}
}
/***********************************************************************
函数名称：void Task_UDP_Client(void *pdata)
功    能：UDP客户端的任务，每隔500ms主动向服务器发送一次数据
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_UDP_Client(void *pdata)
{
	int ret;
	struct sockaddr_in addr;
	struct sockaddr  remote_addr;
	
	client_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);	//生产一个UDP socket号，即用该socket向目标主机发数据
	if (client_sock_fd < 0)								//如果获取失败，打印错误标志
	{
		printf("get socket err:%d\n", errno);				
	} 
	//以下绑定本地端口
	addr.sin_family = AF_INET;					//协议类型  IPV4  internet 协议
	addr.sin_len = sizeof(struct sockaddr_in);	
	addr.sin_port = htons(UDP_LOCAL_PORT);		//将端口号转成网络字节顺序
	addr.sin_addr.s_addr = htons(INADDR_ANY);	//接收所有IP发来的数据
	ret = bind(client_sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));//绑定client_sock_fd
	{
		if(ret < 0)//如果绑定失败
		{
			printf("bind socket err\n");		
		}
	}
	
	//以下设置目标主机地址信息
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(struct sockaddr_in);
	addr.sin_port = htons(UDP_REMOTE_PORT);
	addr.sin_addr.s_addr = inet_addr(UDP_REMOTE_IP);	
	memcpy(&remote_addr, &addr, sizeof(struct sockaddr_in));//装载地址信息到remote_addr
	
	while(1)
	{
		UDP_Sendto_Data(client_sock_fd,"UDP 客户端数据！\n",sizeof("UDP 客户端数据！\n"),remote_addr,0);//注意需要关闭笔记本的防火墙，才能接收到数据包
		OSTimeDlyHMSM(0, 0, 0, 500);						//挂起500ms以便其他任务运行
	}
}

