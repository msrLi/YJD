/***********************************************************************
文件名称：LED.h
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/

#ifndef _LED_H_
#define _LED_H_

#define LED1			GPIOA , GPIO_Pin_4
#define LED2			GPIOA , GPIO_Pin_5
#define LED3			GPIOB , GPIO_Pin_1
#define LED4			GPIOE , GPIO_Pin_11


#define LED1_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_4)
#define LED2_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_5)
#define LED3_ON 		GPIO_ResetBits(GPIOB , GPIO_Pin_1)
#define LED4_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_11)


#define LED1_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_4)
#define LED2_OFF 		GPIO_SetBits(GPIOA , GPIO_Pin_5)
#define LED3_OFF 		GPIO_SetBits(GPIOB , GPIO_Pin_1)
#define LED4_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_11)

void LED_Configuration(void);
void LED_Blink(void);
void One_LED_ON(unsigned char led_num);
static LED_Delay(uint32_t nCount);

#endif
