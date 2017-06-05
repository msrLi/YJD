#include "exti.h"
#include "debug.h"
#include "parallel.h"
extern void parallel_status_PeError();
#define DEBUG
extern int dbg_level;

void EXTI9_5_IRQHandler(void)
{
	YJ_U16	t0;
	int i;
	
	if (EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		if(NULL == pblock)
		{
			printf("%s",__func__);
			EXTI_ClearITPendingBit(EXTI_Line8);
			return;
		}		
		t0 = parallel_read();
		if (!(t0 & 0x100)){
		
			if(pblock->len < BUF_SIZE-1) {
			   pblock->u8Buf[pblock->len++] = t0;
			   pblock->fst = os_time_get();
			}else{
				pblock->len = BUF_SIZE-1 ;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}else{
		printf("other irq is run ...\n");
	}
	//OSSemPost(g_sem_flag);
}

void exti_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	p_info("%s comming", __FUNCTION__);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);

	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置

}

