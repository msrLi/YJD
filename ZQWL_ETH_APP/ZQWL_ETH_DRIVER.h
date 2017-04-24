/***********************************************************************
文件名称：ZQWL_ETH_DRIVER.H
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#ifndef _ZQWL_ETH_DRIVER_H_
#define _ZQWL_ETH_DRIVER_H_

#define TCP_RCV_SIZE 1024


#define DHCP_START_IP       		"192.168.1.100"
#define DHCP_END_IP					"192.168.1.254"
#define POWER_SAVE_MODE				0X02//省电模式
#define OFF_POWER_SAVE_MODE			0X00//退出省电模式

extern struct dhcpMessage *dhcp_packet;
extern struct netif if_wifi;
extern struct netif *p_netif; //一个netif对应一个网络设备
extern int UDP_server_socket_fd;

extern char *BOARD_IP;   			//开发板ip 
extern char *BOARD_NETMASK;   		//开发板子网掩码
extern char *BOARD_WG;   		    //开发板子网关
//socket互斥锁
extern mutex_t socket_mutex;

//用于select操作的变量
extern fd_set rfds;

void ZQWL_System_Clock_Init(void);
int Init_W8782(void);
int Init_Lwip(void);
void tcpip_init_end(void *pdata);
void Enable_Dhcp_Server(void);
int Create_AP(char *ssid,char *password,int key_mode,int channel, int max_client);
int Wifi_Connect(char *ssid,char *password);
void Scan_Wifi(void);
int wifi_scan(void (*scan_result_callback)(struct scan_result_data *res_data),char *essid);
void scan_result_fun(struct scan_result_data *res_data);
int Wifi_Disconnect(void);
int TCP_Link(char *ip,uint16_t port);
int TCP_Unlink(int sock);
int TCP_Send_Data(int s,char *data,int len,int flags);
void TCP_Accept_Task(void *server_fd);
int Create_TCP_Server(uint16_t port);
void tcp_recv_thread(void *arg);
void ZQWL_W8782_IRQ_Handler(int s, uint8_t *buff, int size,struct sockaddr remote_addr);
void Close_TCP_Server(int remote_s,int server_s);
uint32_t DNS(char *hostname);
int get_host_by_name(char *hostname, uint32_t *addr);
extern err_t ethernetif_init(struct netif *netif);
extern err_t tcpip_input(struct pbuf *p, struct netif *inp);
int lwip_netif_init(void);
u32_t Auto_Get_IP(int time);
int Wifi_Get_Status(uint8_t *mac,struct station_info *sinfo);
int UDP_Link(char *ip,uint16_t port);
int UDP_Sendto_Data(int socket,char *data,int len,struct sockaddr remote_addr,int flags);
int UDP_Create_Server(uint16_t port);
int UDP_Create_Client(uint16_t port);
int Wifi_Stop_AP(void);
int Get_Station_List(mlan_ds_sta_list *p_list);
void Wifi_Join_Adhoc(char *ssid, char *key, int ch);
int Wifi_Leave_Adhoc(void);
void MCU_Soft_Reset(void);
int Get_Socket_Status(int socket_num, struct ret_connect_stat *stat);
int Init_Udhcpd(void);
int close_socket(uint32_t socket_num);
#endif

