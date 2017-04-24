// #define DEBUG

#include "main.h"


void delay_1us()
{
	int i = 25;
	while (i--)
		;
}

void delay_us(uint32_t us)
{
	while (us--)
		delay_1us();
}

void assert_failed(uint8_t *file, uint32_t line)
{
	p_err("assert_failed in:%s,line:%d \n", file ? file : "n", line);
	while (1)
		;
}

void HardFault_Handler()
{
#if USE_MEM_DEBUG		
	static int once = 0;
	if (!once)
	{
		once = 1;

		mem_slide_check(0);

	}
#endif
	p_err("%s\n", __FUNCTION__);
// 	printf("HardFault_Handler!");
	while (1)
		;
}


#if OS_APP_HOOKS_EN > 0u
void App_TaskCreateHook(OS_TCB *ptcb)
{
	ptcb = ptcb;
}

void App_TaskDelHook(OS_TCB *ptcb)
{
	ptcb = ptcb;
}

void App_TaskReturnHook(OS_TCB *ptcb)
{
	ptcb = ptcb;
}

void App_TCBInitHook(OS_TCB *ptcb)
{
	ptcb = ptcb;
}

void App_TaskSwHook(void)
{

}

void App_TimeTickHook(void){}

//uC/OS-II Stat线程中调用此函数，每100MS一次
void App_TaskStatHook()
{
	#if USE_MEM_DEBUG
	mem_slide_check(0);
	#endif
	//button_stat_callback();
}

#endif



