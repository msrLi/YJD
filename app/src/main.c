/***********************************************************************
文件名称：main.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#include "main.h"
#define watchdog  1
HANDLER	hParalletMsgQueue;
void *g_qMsgIntTb1[MAX_QUE];

OS_MEM	*g_hParalletRxMem;
INT8U MemPart[MEM_PART][BUF_SIZE];

OS_EVENT  *g_sem_flag = 0;

int mcuID[3];

#define USE_DHCP	0
void iwdg_init(void)
{
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	IWDG_SetPrescaler(4);
	IWDG_SetReload(625);
	IWDG_ReloadCounter();
	IWDG_Enable(); 
}
/***********************************************************************
函数名称：void Init_Wifi(void)
功    能：wifi相关的初始化
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Init_Wifi(void)
{
	int ret;
	u32_t addr;
	ZQWL_System_Clock_Init(); 		//初始化时钟
	OSStatInit(); 					//初始化UCOS状态，必须在初始化时钟之后调用
	USART_Configuration(); 			//初始化串口
	ret = SD_Init();    			//初始化SDIO设备
	if (ret != 0)					//如果失败
	{
		printf("SD_Init faild!\n"); //
		GPIO_SetBits(GPIOC,GPIO_Pin_4);
		while(1);					//程序停止运行
	}
	ret = Init_W8782();				//初始化WIFI芯片
	if(ret != 0)					//如果失败
	{
		printf("init wifi faild!\n");  
		GPIO_SetBits(GPIOC,GPIO_Pin_4);
		while(1);					//程序停止运行
	}
	printf("Init_W8782 ok!\n");  
	Power_Mode(OFF_POWER_SAVE_MODE);	//不 进入省电模式，OFF_POWER_SAVE_MODE 为退出省电模式
	Init_Lwip(); 					//初始化lwip协议栈，并初始化了板子的IP

#if USE_DHCP
	Auto_Get_IP(3);
#endif
	
	//以下创建一个AP
// 	ret = Init_Udhcpd(); 			//初始化dhcp服务器，如果WIFI工作的STA模式下（与路由器连接）可以不用DHCP
// 	if(ret == -1)
// 	{
// 		printf("Init_Udhcpd faild!\n");
// 		while(1);
// 	}	
// 	printf("Init_Udhcpd ok!\n");  
// 	Enable_Dhcp_Server(); 			// 开启dhcp服务器,如果工作在sta模式,可以不开启dhcpserver	
// 	ret = Create_AP("ZQWL","12345678",KEY_WPA2,6,4);//创建AP，创建AP后就不要再调Wifi_Connect函数了
// 	if(ret == -1)
// 	{
// 		printf("Create_AP faild!\n");
// 		while(1);
// 	}
// 	printf("Create_AP ok!\n");  //
	
	//以下加入一个AP（路由器）
	ret = Wifi_Connect("yixiaoke","57018092");
	// ret = Wifi_Connect("TPGuest_1425","xwz145603");
	//ret = Wifi_Connect("king","zhouyangdelaoer");//路由器名称和密码，该函数调用后，自动进入sta模式
	if(ret == -1)
	{
		printf("Wifi_Connect faild!\n");
		while(1);
	}
	printf("Wifi_Connect ok!\n");  //
}

void task_led(void *pdata)
{
#if watchdog
	iwdg_init();            // satrt watch dog 
#endif 
	while(1)
	{
#if watchdog
		IWDG_ReloadCounter(); // fead the watch dog
#endif 
		GPIO_SetBits(GPIOC,GPIO_Pin_4);
		sleep(500);
#if watchdog
		IWDG_ReloadCounter(); // fead the watch dog 
#endif 
		GPIO_ResetBits(GPIOC,GPIO_Pin_4);
		sleep(500);
	}
}

void task_uart(void *pdata)
{
	INT8U 			err;
	parallet_msg_t	*pblock = NULL;
	OS_Q_DATA  		qInfo;
	int				i;
	
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
					for (i=0; i< pblock->len; i++)
						printf("%c",pblock->u8Buf[i]);
					OSMemPut(g_hParalletRxMem, pblock);
				}
			}
		}
		OSTimeDly(1);
	}
}

void cpuidGetId(void)
{
    mcuID[0] = *(__IO u32*)(0x1FFF7A10);
    mcuID[1] = *(__IO u32*)(0x1FFF7A14);
    mcuID[2] = *(__IO u32*)(0x1FFF7A18);
}

/***********************************************************************
函数名称：void main_thread(void *pdata)
功    能：主线程
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void main_thread(void *pdata)
{
	INT8U u8Err;
	
	Init_Wifi();
	
	cpuidGetId();
	printf("CPUID IS 0x%8X %8X %8X.\r\n", mcuID[0], mcuID[1], mcuID[2] );
	
	printf("\r\n***************************************************\r\n");

#ifdef VERSION
	printf("         Version 	%s \r\n", VERSION);
#else
	printf("         Version 	DEBUG Version\r\n");
#endif
	printf("         Built at %s in %s\r\n", __TIME__, __DATE__);
	printf("***************************************************\r\n\r\n");

	LED_Configuration();	
	init_parallel();
	exti_init();

	hParalletMsgQueue = OSQCreate(&g_qMsgIntTb1[0], MAX_QUE);
	LWIP_ASSERT(".... hCan1RxMsgQueue queue created failed !\n\r", hParalletMsgQueue != NULL );

	g_hParalletRxMem = OSMemCreate(MemPart, MEM_PART, BUF_SIZE+6, &u8Err);

	g_sem_flag = OSSemCreate(0);
	thread_create(task_parallet, 0, TASK_PARALLEL_PRIO, 0, TASK_PARALLEL_STK_SIZE, "task_parallet");
	thread_create(Task_TCP_Client, 0, TASK_TCP_CLIENT_PRIO, 0, TASK_TCP_CLIENT_STK_SIZE, "Task_TCP_Client");
	thread_create(task_led, 0, TASK_LED_PRIO, 0, TASK_LED_STK_SIZE, "task_led");
	//thread_create(task_uart, 0, TASK_UART_PRIO, 0, TASK_UART_STK_SIZE, "task_uart");
	
	
	while (1) //在此可以指定一个CPU运行指示灯闪烁，监视系统运行状态
	{
		OSTimeDlyHMSM(0, 0, 1, 0); //
	}
}
/***********************************************************************
函数名称：int main(void)
功    能：程序入口
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
int main(void)
{
	OSInit();

	_mem_init(); //初始化内存分配

	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");

	OSStart();
	return 0;
}
