/***********************************************************************
文件名称：LED_Ctrl.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：

***********************************************************************/
#include "main.h"

/***********************************************************************
函数名称：void LED_Ctrl(unsigned char *data)
功    能：根据data的数据命令控制led的亮灭
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
			命令：	led1_on				LED1灯亮
					led2_on				LED2灯亮
					led3_on				LED3灯亮
					led4_on				LED4灯亮
									
					led1_off			LED1灯灭
					led2_off			LED2灯灭
					led3_off			LED3灯灭
					led4_off			LED4灯灭
***********************************************************************/
void LED_Ctrl(unsigned char *data)
{
	if(strncmp(&data[0],"led", 3) == 0)//是LED的控制数据
	{
		switch(data[3])				//第3个字节是控制哪个LED的
		{
			case '1':
			{
				if(strncmp(&data[4],"_on", 3) == 0)//如果是led1_on
				{
					LED1_ON;
				}
				else if(strncmp(&data[4],"_off", 4) == 0)
				{
					LED1_OFF;

				}
				break;
			}
			case '2':
			{
				if(strncmp(&data[4],"_on", 3) == 0)//如果是led1_on
				{
					LED2_ON;
				}
				else if(strncmp(&data[4],"_off", 4) == 0)
				{
					LED2_OFF;

				}
				break;
			}
			case '3':
			{
				if(strncmp(&data[4],"_on", 3) == 0)//如果是led1_on
				{
					LED3_ON;
				}
				else if(strncmp(&data[4],"_off", 4) == 0)
				{
					LED3_OFF;

				}
				break;
			}
			case '4':
			{
				if(strncmp(&data[4],"_on", 3) == 0)//如果是led1_on
				{
					LED4_ON;
				}
				else if(strncmp(&data[4],"_off", 4) == 0)
				{
					LED4_OFF;

				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}