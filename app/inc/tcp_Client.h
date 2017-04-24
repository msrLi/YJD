/***********************************************************************
文件名称：TCP_CLient.h
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

void Init_Wifi(void);
void Task_TCP_Client(void *pdata);
void Task_Client_Sock_Stats(void *pdata);


#endif

