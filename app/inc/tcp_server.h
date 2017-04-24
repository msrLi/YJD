/***********************************************************************
文件名称：TCP_Server.h
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

extern char *BOARD_IP ;   			//开发板ip 
extern char *BOARD_NETMASK;   		//开发板子网掩码
extern char *BOARD_WG;   		    //开发板子网关

void Task_TCP_server(void *pdata);

#endif



