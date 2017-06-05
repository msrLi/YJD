/****************************** copyrigth (c) 2015 ***************************
 * Filename     : parallel.c
 * Compiler     : coding--GB2312(SourceInsight)
 *                keil v5.11
 * Author       : yangmin
 * Time         : 2015-01-03
 * Description  : 并口(SPP模式)相关操作实现。一共17条信号线。
 *****************************************************************************/
#include "parallel.h"

#define	ATOB 	0
#define	BTOA	1

#define MCU_TO_PLT	ATOB
#define PLT_TO_MCU	BTOA

#define MCU_WRITE	MCU_TO_PLT
#define MCU_READ	PLT_TO_MCU

struct BSEG
{
	YJ_U8 bit0:    1;
	YJ_U8 bit1:    1;
	YJ_U8 bit2:    1;
	YJ_U8 bit3:    1;
	YJ_U8 bit4:    1;
	YJ_U8 bit5:    1;
	YJ_U8 bit6:    1;
	YJ_U8 bit7:    1;
};

union DATA
{
	YJ_U8 ch;
	struct BSEG byte;
};

typedef struct _parallel_t
{
	union DATA data;
	union DATA status;
} parallet_t,*parallet_p;

parallet_t g_parallet;

static YJ_BOOL parallel_bit_status(YJ_U16 u16Type);

static void is_sn74hc245u1u5_enable(YJ_BOOL bool)
{
	if (bool)
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	else
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
}

static void sn74hc245u1u5_direction(YJ_S8 s8Flag)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	if (MCU_WRITE == s8Flag) //mcu输出
	{
		//GPIOD8,9,10,11,12,13,14,15初始化设置:推挽输出
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		GPIO_SetBits(GPIOC,GPIO_Pin_6);
	}
	else // mcu输入
	{
		//GPIOD8,9,10,11,12,13,14,15初始化设置: 上拉输入
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	}
}

static void is_sn74hc245u2_enable(YJ_BOOL bool)
{
	if (bool)
		GPIO_ResetBits(GPIOB, MCU_OE1);
	else
		GPIO_SetBits(GPIOB,MCU_OE1);
}

/*Status Port (Read only). only BTOA*/
static void sn74hc245u2_direction(YJ_S8	s8Flag)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if (MCU_WRITE == s8Flag) // mcu输出
	{	
		//GPIOE10,11,12,13,14初始化设置:推挽输出
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		GPIO_SetBits(GPIOE,MCU_DIR1);
	}
	else //mcu输入
	{
		//GPIOE10,11,12,13,14初始化设置: 上拉输入
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		GPIO_ResetBits(GPIOE,MCU_DIR1);
	}	
}

static void is_init_data_control_line(YJ_BOOL bool)
{
	is_sn74hc245u1u5_enable(bool);
}

static void is_init_status_line(YJ_BOOL bool)
{
	is_sn74hc245u2_enable(bool);
}

static void set_data_control_direction(YJ_S8 s8Flag)
{
	sn74hc245u1u5_direction(s8Flag);
}

static void set_status_line_direction(YJ_S8 s8Flag)
{
	sn74hc245u2_direction(s8Flag);
}

static void init_sn74hc245u1(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOC6,7初始化设置:推挽输出 MCU_OE MCU_DIR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//GPIOB11 初始化设置:推挽输出 MCU_OE1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIOE15 初始化设置:推挽输出 MCU_DIR1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
  GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	// is_init_data_control_line(YJ_TRUE);
	// is_init_status_line(YJ_TRUE);

	set_data_control_direction(MCU_READ);
	set_status_line_direction(MCU_READ);
}

YJ_S8 init_parallel(void)
{
	YJ_S8 s8Ret=YJ_TRUE;
 
	init_sn74hc245u1();
	return s8Ret;
}
void parallel_status_PeError()
{
	set_status_line_direction(MCU_WRITE);
	parallel_status_write(MCU_PERROR, YJ_FALSE);
	set_status_line_direction(MCU_READ);
}
YJ_U16 parallel_read(void)
{
	YJ_BOOL bRet;
	YJ_U16 u8Data=0x1FF;
	YJ_U16 timeout=0;
	g_parallet.data.ch = 0;
	
	bRet = parallel_bit_status(MCU_STROBE);
	if (YJ_FALSE == bRet)
	{
		g_parallet.data.byte.bit0 = GPIO_ReadInputDataBit(GPIOD, DAT0);
		g_parallet.data.byte.bit1 = GPIO_ReadInputDataBit(GPIOD, DAT1);
		g_parallet.data.byte.bit2 = GPIO_ReadInputDataBit(GPIOD, DAT2);
		g_parallet.data.byte.bit3 = GPIO_ReadInputDataBit(GPIOD, DAT3);
		g_parallet.data.byte.bit4 = GPIO_ReadInputDataBit(GPIOD, DAT4);
		g_parallet.data.byte.bit5 = GPIO_ReadInputDataBit(GPIOD, DAT5);
		g_parallet.data.byte.bit6 = GPIO_ReadInputDataBit(GPIOD, DAT6);
		g_parallet.data.byte.bit7 = GPIO_ReadInputDataBit(GPIOD, DAT7);
		u8Data= g_parallet.data.ch & 0xFF;
	}
	
	return u8Data;
}

YJ_U16 sim_parallel_read(void)
{
	YJ_BOOL bRet;
	YJ_U16 u8Data=0x1FF;

	bRet = parallel_bit_status(MCU_STROBE);
	if (YJ_FALSE == bRet)
	{
		g_parallet.data.byte.bit0 = GPIO_ReadInputDataBit(GPIOD, DAT0);
		g_parallet.data.byte.bit1 = GPIO_ReadInputDataBit(GPIOD, DAT1);
		g_parallet.data.byte.bit2 = GPIO_ReadInputDataBit(GPIOD, DAT2);
		g_parallet.data.byte.bit3 = GPIO_ReadInputDataBit(GPIOD, DAT3);
		g_parallet.data.byte.bit4 = GPIO_ReadInputDataBit(GPIOD, DAT4);
		g_parallet.data.byte.bit5 = GPIO_ReadInputDataBit(GPIOD, DAT5);
		g_parallet.data.byte.bit6 = GPIO_ReadInputDataBit(GPIOD, DAT6);
		g_parallet.data.byte.bit7 = GPIO_ReadInputDataBit(GPIOD, DAT7);
		parallel_status_write(MCU_BUSY, YJ_TRUE);
		parallel_status_write(MCU_ACK, YJ_FALSE);
		delay_us(1);
		u8Data = g_parallet.data.ch & 0xFF;

		parallel_status_write(MCU_ACK, YJ_TRUE);
	}
	
	return u8Data;
}

void sim_parallel_write(YJ_U8 u8Data)
{
	union DATA data;

	data.ch = u8Data;
	GPIO_WriteBit(GPIOD, DAT0, data.byte.bit0);
	GPIO_WriteBit(GPIOD, DAT1, data.byte.bit1);
	GPIO_WriteBit(GPIOD, DAT2, data.byte.bit2);
	GPIO_WriteBit(GPIOD, DAT3, data.byte.bit3);
	GPIO_WriteBit(GPIOD, DAT4, data.byte.bit4);
	GPIO_WriteBit(GPIOD, DAT5, data.byte.bit5);
	GPIO_WriteBit(GPIOD, DAT6, data.byte.bit6);
	GPIO_WriteBit(GPIOD, DAT7, data.byte.bit7);
}

static YJ_BOOL parallel_bit_status(YJ_U16 u16Type)
{
	YJ_BOOL bRet;
	union DATA data;

	data.ch = 0x0;
	switch(u16Type)
	{
		case MCU_SLCT:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_SLCT);
			break;
		case MCU_ERROR:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_ERROR);
			break;
		case MCU_PERROR:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_PERROR);
			break;
		case MCU_BUSY:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_BUSY);;
			break;
		case MCU_ACK:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_ACK);
			break;
		case MCU_STROBE:
			data.byte.bit0 = GPIO_ReadInputDataBit(GPIOE, MCU_STROBE);
			break;
		default:
			printf("read status 0x%x no define!\n", u16Type);
			break;
	}

	if (1 == data.byte.bit0)
		bRet = YJ_TRUE;
	else
		bRet = YJ_FALSE;

	return bRet;
}

void parallel_status_write(YJ_U16 u16Type, YJ_BOOL bValue)
{
	union DATA data;

	ASSERT(IS_MCU_STATUS(u16Type));

	if (YJ_TRUE == bValue)
		data.byte.bit0 = 1;
	else
		data.byte.bit0 = 0;

	GPIO_WriteBit(GPIOE, u16Type, data.byte.bit0);
}

void check_status()
{
	YJ_BOOL bRet;
	
	bRet = parallel_bit_status(MCU_SLCT);
	if (YJ_TRUE == bRet)
	{
		printf("MCU_SLCT is 1!\r\n");
	}
	else
	{
		printf("MCU_SLCT is 0!\r\n");
	}

	bRet = parallel_bit_status(MCU_ERROR);
	if (YJ_TRUE == bRet)
	{
		printf("MCU_ERROR is 1!\r\n");
	}
	else
	{
		printf("MCU_ERROR is 0!\r\n");
	}

	bRet = parallel_bit_status(MCU_PERROR);
	if (YJ_TRUE == bRet)
	{
		printf("MCU_PERROR is 1!\r\n");
	}
	else
	{
		printf("MCU_PERROR is 0!\r\n");
	}

	bRet = parallel_bit_status(MCU_BUSY);
	if (YJ_TRUE == bRet)
	{
		printf("MCU_BUSY is 1!\r\n");
	}
	else
	{
		printf("MCU_BUSY is 0!\r\n");
	}

	bRet = parallel_bit_status(MCU_ACK);
	if (YJ_TRUE == bRet)
	{
		printf("MCU_ACK is 1!\r\n");
	}
	else
	{
		printf("MCU_ACK is 0!\r\n");
	}
}

