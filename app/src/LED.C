/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

/***********************************************************************
函数名称：LED_Configuration(void)
功    能：完成LED的配置
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
void LED_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOC4初始化设置:推挽输出 LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************
函数名称：LED_Blink(void)
功    能：完成LED闪烁
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
void LED_Blink(void)
{
	/*****熄灭四个led灯******/
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	LED_Delay(0x4fffff);
	/*****点亮四个led灯******/
	LED1_ON;
	LED2_ON;
	LED3_ON;
	LED4_ON;
	LED_Delay(0x4fffff);
}
/***********************************************************************
函数名称：One_LED_ON(unsigned char led_num)
功    能：实现点亮一个LED灯
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
void One_LED_ON(unsigned char led_num)
{	
	/*****熄灭四个led灯******/
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	switch(led_num)
	{
		case 1:
		{
			LED1_ON;
			break;
		}
		case 2:
		{
			LED2_ON;
			break;		
		}
		case 3:
		{
			LED3_ON;
			break;		
		}
		case 4:
		{
			LED4_ON;
			break;		
		}
		default:
		{
			break;	
		}
	}		
}

static LED_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}
