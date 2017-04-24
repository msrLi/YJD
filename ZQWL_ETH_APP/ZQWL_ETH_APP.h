/***********************************************************************
文件名称：ZQWL_ETH_APP.h
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
 
#ifndef _ZQWL_ETH_APP_H_
#define _ZQWL_ETH_APP_H_

#include "drivers.h"
#include "app.h"
#include "api.h"
#include "moal_main.h"
#include "moal_uap.h"
#include "moal_uap_priv.h"
#include "mlan_ioctl.h"
#include "lwip\sockets.h"
#include "lwip\netif.h"
#include "lwip\dns.h"
#include "lwip\api.h"
#include "lwip\tcp.h"
#include "lwip/tcpip.h"

// #include "cfg80211.h"
// #include "defs.h"
// #include "type.h"
// #include "types.h"

// #include "tcpapp.h"
#include "wifi.h"
// #include "debug.h"
#include "lwip/dhcp.h"
#include "dhcpd.h"
// #include "sys_misc.h"


// #include <cctype>
#include "moal_sdio.h"
#include "lwip/pbuf.h"
// #include "lwip/sys.h"
// #include "wpa.h"
#include "ZQWL_ETH_DRIVER.h"
#include "WIFI_CONFIG.h"

void delay_1us(void);
void delay_us(uint32_t us);
void Init_Wifi(void);

#endif
