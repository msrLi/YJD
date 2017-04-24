#include "main.H"

#include "drivers.h"
#include "app.h"
#include "api.h"

#include <absacc.h>

extern char Image$$RW_IRAM1$$ZI$$Limit[];
void _mem_init(void)
{
	uint32_t malloc_start, malloc_size;

	malloc_start = (uint32_t) &Image$$RW_IRAM1$$ZI$$Limit; //取空闲内存最低地址
	malloc_start = (malloc_start + 3) & (~0x03);// ALIGN TO word

//0x20000为板子内存总大小，更换MCU时需注意
	malloc_size = 0x20000000 + 0x20000 - malloc_start;

	sys_meminit((void*)malloc_start, malloc_size);
}

void init_systick()
{

}

void init_rng()
{

}

/**
 *读一个随机数
 */
uint32_t get_random(void)
{
	return RNG_GetRandomNumber();
}



void driver_gpio_init()
{



}


int check_rst_stat()
{
	uint32_t stat;
	stat = RCC->CSR;
	RCC->CSR = 1L << 24; //清除复位标志

	p_err("reset:");
	if (stat &(1UL << 31))
	// 低功耗复位
	{
		p_err("low power\n");
	}
	if (stat &(1UL << 30))
	//窗口看门狗复位
	{
		p_err("windw wdg\n");
	}
	if (stat &(1UL << 29))
	//独立看门狗复位
	{
		p_err("indep wdg\n");
	}
	if (stat &(1UL << 28))
	//软件复位
	{
		p_err("soft reset\n");
	}
	if (stat &(1UL << 27))
	//掉电复位
	{
		p_err("por reset\n");
	}
	if (stat &(1UL << 26))
	//rst复位
	{
		p_err("user reset\n");
	}

	return 0;
}



