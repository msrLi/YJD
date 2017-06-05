/***********************************************************************
�ļ����ƣ�main.C
��    �ܣ�
��дʱ�䣺
�� д �ˣ�������Ƕ���������Ӽ����Ŷ�
ע    �⣺
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
�������ƣ�void Init_Wifi(void)
��    �ܣ�wifi��صĳ�ʼ��
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
void Init_Wifi(void)
{
	int ret;
	u32_t addr;
	ZQWL_System_Clock_Init(); 		//��ʼ��ʱ��
	OSStatInit(); 					//��ʼ��UCOS״̬�������ڳ�ʼ��ʱ��֮�����
	USART_Configuration(); 			//��ʼ������
	ret = SD_Init();    			//��ʼ��SDIO�豸
	if (ret != 0)					//���ʧ��
	{
		printf("SD_Init faild!\n"); //
		GPIO_SetBits(GPIOC,GPIO_Pin_4);
		while(1);					//����ֹͣ����
	}
	ret = Init_W8782();				//��ʼ��WIFIоƬ
	if(ret != 0)					//���ʧ��
	{
		printf("init wifi faild!\n");  
		GPIO_SetBits(GPIOC,GPIO_Pin_4);
		while(1);					//����ֹͣ����
	}
	printf("Init_W8782 ok!\n");  
	Power_Mode(OFF_POWER_SAVE_MODE);	//�� ����ʡ��ģʽ��OFF_POWER_SAVE_MODE Ϊ�˳�ʡ��ģʽ
	Init_Lwip(); 					//��ʼ��lwipЭ��ջ������ʼ���˰��ӵ�IP

#if USE_DHCP
	Auto_Get_IP(3);
#endif
	
	//���´���һ��AP
// 	ret = Init_Udhcpd(); 			//��ʼ��dhcp�����������WIFI������STAģʽ�£���·�������ӣ����Բ���DHCP
// 	if(ret == -1)
// 	{
// 		printf("Init_Udhcpd faild!\n");
// 		while(1);
// 	}	
// 	printf("Init_Udhcpd ok!\n");  
// 	Enable_Dhcp_Server(); 			// ����dhcp������,���������staģʽ,���Բ�����dhcpserver	
// 	ret = Create_AP("ZQWL","12345678",KEY_WPA2,6,4);//����AP������AP��Ͳ�Ҫ�ٵ�Wifi_Connect������
// 	if(ret == -1)
// 	{
// 		printf("Create_AP faild!\n");
// 		while(1);
// 	}
// 	printf("Create_AP ok!\n");  //
	
	//���¼���һ��AP��·������
	ret = Wifi_Connect("yixiaoke","57018092");
	// ret = Wifi_Connect("TPGuest_1425","xwz145603");
	//ret = Wifi_Connect("king","zhouyangdelaoer");//·�������ƺ����룬�ú������ú��Զ�����staģʽ
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
�������ƣ�void main_thread(void *pdata)
��    �ܣ����߳�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
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
	
	
	while (1) //�ڴ˿���ָ��һ��CPU����ָʾ����˸������ϵͳ����״̬
	{
		OSTimeDlyHMSM(0, 1, 0, 0); //
	}
}
/***********************************************************************
�������ƣ�int main(void)
��    �ܣ��������
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
int main(void)
{
	OSInit();

	_mem_init(); //��ʼ���ڴ����

	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");

	OSStart();
	return 0;
}
