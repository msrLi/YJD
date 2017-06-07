/***********************************************************************
文件名称：main.h
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
 
#ifndef _MAIN_H_
#define _MAIN_H_

#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_iwdg.h"
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

#include "cfg80211.h"
#include "defs.h"
#include "type.h"
#include "types.h"

#include "tcpapp.h"
#include "wifi.h"
#include "debug.h"
#include "lwip/dhcp.h"
#include "dhcpd.h"
#include "sys_misc.h"

#include "dhcpd.h"

#include <cctype>
#include "moal_sdio.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "wpa.h"
#include "SCI.h"
#include "LED.h"
#include "LED_Ctrl.h"

#include "tcp_Client.h"
#include "tcp_Server.h"
#include "UDP_Client.h"
#include "UDP_Server.h"
#include "ZQWL_ETH_APP.h"
#include "task_parallet.h"
#include "exti.h"

void delay_1us(void);
void delay_us(uint32_t us);
void Init_Wifi(void);

#define		MAX_QUE		12
#define		MEM_PART	MAX_QUE

extern HANDLER	hParalletMsgQueue;
extern void *g_qMsgIntTb1[MAX_QUE];

extern OS_MEM	*g_hParalletRxMem;
extern OS_MEM  *g_hParalletNewRxMem;
extern OS_MEM *g_hParalletHeadMem;

extern INT8U MemPart[MEM_PART][BUF_SIZE];
extern OS_EVENT  *g_sem_flag;

#endif

