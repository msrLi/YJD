/***********************************************************************
文件名称：wifi_config.h
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_



// #define DEBUG_LEVEL1
//#define DEBUG_LEVEL2

#ifdef STA_SUPPORT	
extern struct net_device *sta_wifi_dev ;
#endif
#ifdef UAP_SUPPORT	
extern struct net_device *ap_wifi_dev;
#endif
#ifdef WIFI_DIRECT_SUPPORT
extern struct net_device *direct_wifi_dev;
#endif
extern struct net_device *wifi_dev;

void Fix_ZQWL_Params(void);
int Power_Mode(char mode);
void print_scan_result(struct scan_result_data *dat);

#endif
