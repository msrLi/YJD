/****************************** copyrigth (c) 2015 ***************************
 * Filename     : parallel.h
 * Compiler     : coding--GB2312(SourceInsight)
 *                keil v5.11
 * Author       : yangmin
 * Time         : 2015-12-29
 * Description  : 并口相关操作
 *****************************************************************************/
#ifndef	_YJD_DRIVERS_PARALLEL_H_
#define	_YJD_DRIVERS_PARALLEL_H_

#include "drivers.h"
#include "app.h"
#include "api.h"


/*并口数据线定义(8条)*/
#define	DAT0	GPIO_Pin_15 //PD15 LPT_PIN2 单向数据线
#define	DAT1	GPIO_Pin_14 //PD14 LPT_PIN3
#define	DAT2	GPIO_Pin_13 //PD13 LPT_PIN4
#define	DAT3	GPIO_Pin_12 //PD12 LPT_PIN5
#define	DAT4	GPIO_Pin_11 //PD11 LPT_PIN6
#define	DAT5	GPIO_Pin_10 //PD10 LPT_PIN7
#define	DAT6	GPIO_Pin_9  //PD09 LPT_PIN8
#define	DAT7	GPIO_Pin_8  //PD08 LPT_PIN9

/*并口控制线(Control Port)定义(4条)*/
#define MCU_STROBE	GPIO_Pin_8 //PE8 LPT_PIN01 主机;在有效低脉冲时段上数据有效
#define MCU_AUTOLF	GPIO_Pin_2 //PB2 LPT_PIN14 主机;自动换行。当为低时,多数打印机在每个回车后执行换行;为高时只执行回车 (可能有不同定义)
#define MCU_INITPR	GPIO_Pin_1 //PB1 LPT_PIN16 主机;初始化打印机。信号保持低大于50us将复位打印机并清除打印机缓冲区
#define MCU_SLCTIN	GPIO_Pin_7 //PE7 LPT_PIN17 主机;选择打印机。主机设置信号为低选择外设

/*并口状态线(Status Port)定义(5条readonly)*/
#define MCU_ERROR   GPIO_Pin_10 //PE10 LPT_PIN15 外设;外设通常在有存在的错误时设为低 (可能有不同定义)
#define MCU_SLCT	GPIO_Pin_11 //PE11 LPT_PIN13 外设;外设在被接入且准备好数据传输时，设为高
#define MCU_PERROR	GPIO_Pin_12 //PE12 LPT_PIN12 外设;打印机缺纸时一般设置为高 (可能有不同定义)
#define MCU_BUSY	GPIO_Pin_13 //PE13 LPT_PIN11 外设;外设设为高表示未准备好接收数据
#define MCU_ACK		GPIO_Pin_14 //PE14 LPT_PIN10 外设;外设产生低脉冲表示已接收到数据并准备好接收下一个数据，其上升沿可作为对主机的中断。

#define MCU_OE		GPIO_Pin_7 //PC07 针对sn74hc245数据线和控制线
#define MCU_DIR		GPIO_Pin_6 //PC06 针对sn74hc245数据线和控制线

#define MCU_OE1   	GPIO_Pin_11 //PB11 针对sn74hc245状态线
#define MCU_DIR1	GPIO_Pin_15 //PE15 针对sn74hc245状态线

#define IS_MCU_STATUS(PIN)	(((PIN) == MCU_ERROR)	|| \
							 ((PIN) == MCU_SLCT) 	|| \
							 ((PIN) == MCU_PERROR) 	|| \
							 ((PIN) == MCU_BUSY) 	|| \
							 ((PIN) == MCU_ACK))
#define MCU_SLCT_BIT	BIT(0)
#define	MCU_ERROR_BIT	BIT(1)
#define	MCU_PERROR_BIT	BIT(2)	
#define MCU_BUSY_BIT	BIT(3)
#define	MCU_ACK_BIT		BIT(4)

#ifdef __cplusplus
extern "C" {
#endif

YJ_S8 init_parallel(void);
void parallel_status_write(YJ_U16 u16Type, YJ_BOOL bValue);

#ifdef __cplusplus
}
#endif

#endif
