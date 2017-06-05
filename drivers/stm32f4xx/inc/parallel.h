/****************************** copyrigth (c) 2015 ***************************
 * Filename     : parallel.h
 * Compiler     : coding--GB2312(SourceInsight)
 *                keil v5.11
 * Author       : yangmin
 * Time         : 2015-12-29
 * Description  : ������ز���
 *****************************************************************************/
#ifndef	_YJD_DRIVERS_PARALLEL_H_
#define	_YJD_DRIVERS_PARALLEL_H_

#include "drivers.h"
#include "app.h"
#include "api.h"


/*���������߶���(8��)*/
#define	DAT0	GPIO_Pin_15 //PD15 LPT_PIN2 ����������
#define	DAT1	GPIO_Pin_14 //PD14 LPT_PIN3
#define	DAT2	GPIO_Pin_13 //PD13 LPT_PIN4
#define	DAT3	GPIO_Pin_12 //PD12 LPT_PIN5
#define	DAT4	GPIO_Pin_11 //PD11 LPT_PIN6
#define	DAT5	GPIO_Pin_10 //PD10 LPT_PIN7
#define	DAT6	GPIO_Pin_9  //PD09 LPT_PIN8
#define	DAT7	GPIO_Pin_8  //PD08 LPT_PIN9

/*���ڿ�����(Control Port)����(4��)*/
#define MCU_STROBE	GPIO_Pin_8 //PE8 LPT_PIN01 ����;����Ч������ʱ����������Ч
#define MCU_AUTOLF	GPIO_Pin_2 //PB2 LPT_PIN14 ����;�Զ����С���Ϊ��ʱ,������ӡ����ÿ���س���ִ�л���;Ϊ��ʱִֻ�лس� (�����в�ͬ����)
#define MCU_INITPR	GPIO_Pin_1 //PB1 LPT_PIN16 ����;��ʼ����ӡ�����źű��ֵʹ���50us����λ��ӡ���������ӡ��������
#define MCU_SLCTIN	GPIO_Pin_7 //PE7 LPT_PIN17 ����;ѡ���ӡ�������������ź�Ϊ��ѡ������

/*����״̬��(Status Port)����(5��readonly)*/
#define MCU_ERROR   GPIO_Pin_10 //PE10 LPT_PIN15 ����;����ͨ�����д��ڵĴ���ʱ��Ϊ�� (�����в�ͬ����)
#define MCU_SLCT	GPIO_Pin_11 //PE11 LPT_PIN13 ����;�����ڱ�������׼�������ݴ���ʱ����Ϊ��
#define MCU_PERROR	GPIO_Pin_12 //PE12 LPT_PIN12 ����;��ӡ��ȱֽʱһ������Ϊ�� (�����в�ͬ����)
#define MCU_BUSY	GPIO_Pin_13 //PE13 LPT_PIN11 ����;������Ϊ�߱�ʾδ׼���ý�������
#define MCU_ACK		GPIO_Pin_14 //PE14 LPT_PIN10 ����;��������������ʾ�ѽ��յ����ݲ�׼���ý�����һ�����ݣ��������ؿ���Ϊ���������жϡ�

#define MCU_OE		GPIO_Pin_7 //PC07 ���sn74hc245�����ߺͿ�����
#define MCU_DIR		GPIO_Pin_6 //PC06 ���sn74hc245�����ߺͿ�����

#define MCU_OE1   	GPIO_Pin_11 //PB11 ���sn74hc245״̬��
#define MCU_DIR1	GPIO_Pin_15 //PE15 ���sn74hc245״̬��

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
YJ_U16 parallel_read(void);
#ifdef __cplusplus
}
#endif

#endif
