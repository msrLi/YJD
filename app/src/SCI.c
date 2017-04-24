
#include "main.H"
/***********************************************************************
ÎÄ¼şÃû³Æ£ºSCI.C
¹¦    ÄÜ£ºÍê³É¶Ôusart1ºÍusart2µÄ²Ù×÷
±àĞ´Ê±¼ä£º2012.11.22
±à Ğ´ ÈË£º
×¢    Òâ£º±¾Àı³ÌÊÇÍ¨¹ıÅĞ¶ÏÁ½¸öÌØ¶¨µÄ×Ö·ûÀ´È·¶¨Ò»Ö¡Êı¾İÊÇ·ñ½áÊøµÄ¡£
USART1 Ê±ÖÓ : RCC_APB2PeriphClockCmd
USART1~6 Ê±ÖÓ :RCC_APB1PeriphClockCmd
***********************************************************************/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;
}
volatile unsigned char RS232_REC_Flag = 0;
volatile unsigned char RS485_REC_Flag = 0;
volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE];//ÓÃÓÚ½ÓÊÕÊı¾İ
volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//ÓÃÓÚ½ÓÊÕÊı¾İ
volatile unsigned int RS232_rec_counter = 0;//ÓÃÓÚRS232½ÓÊÕ¼ÆÊı
volatile unsigned int RS485_rec_counter = 0;//ÓÃÓÚRS485½ÓÊÕ¼ÆÊı

void USART_Configuration(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure;//¶¨ÒåGPIO_InitTypeDefÀàĞÍµÄ½á¹¹Ìå³ÉÔ±GPIO_InitStructure

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	//Ê¹ÄÜĞèÒªÓÃµ½µÄGPIO¹Ü½ÅÊ±ÖÓ
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);
	//Ê¹ÄÜUSART1 Ê±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	///¸´Î»´®¿Ú1
	USART_DeInit(USART1);
	
	USART_StructInit(&USART_InitStructure);//ÔØÈëÄ¬ÈÏUSART²ÎÊı
	USART_ClockStructInit(&USART_ClockInitStruct);//ÔØÈëÄ¬ÈÏUSART²ÎÊı        
	//ÅäÖÃ´®¿Ú1µÄ¹Ü½Å PA8 USART1_EN PA9 USART1_TX PA10 USART1_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //¸´ÓÃ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //¹Ü½ÅPA9¸´ÓÃÎªUSART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_ClockInit(USART1,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure); 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);        ///////½ÓÊÕÖĞ¶ÏÊ¹ÄÜ
	USART_ClearITPendingBit(USART1, USART_IT_TC);//Çå³ıÖĞ¶ÏTCÎ»
	USART_Cmd(USART1,ENABLE);//×îºóÊ¹ÄÜ´®Ú
}

/***********************************************************************
º¯ÊıÃû³Æ£ºvoid USART1_IRQHandler(void) 
¹¦    ÄÜ£ºÍê³ÉSCIµÄÊı¾İµÄ½ÓÊÕ£¬²¢×ö±êÊ¶
ÊäÈë²ÎÊı£º
Êä³ö²ÎÊı£º
±àĞ´Ê±¼ä£º2012.11.22
±à Ğ´ ÈË£º
×¢    Òâ  RS232ÓÃµÄÊÇUSART1.
***********************************************************************/
void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		RS232_buff[RS232_rec_counter] = USART1->DR;//
		RS232_rec_counter ++;
/********ÒÔRS232_END_FLAG1ºÍRS232_END_FLAG2¶¨ÒåµÄ×Ö·û×÷ÎªÒ»Ö¡Êı¾İµÄ½áÊø±êÊ¶************/
		if(RS232_rec_counter >= 2)	//Ö»ÓĞ½ÓÊÕµ½2¸öÊı¾İÒÔÉÏ²Å×öÅĞ¶Ï
		{
			if(RS232_buff[RS232_rec_counter - 2] == RS232_END_FLAG1 && RS232_buff[RS232_rec_counter - 1] == RS232_END_FLAG2) 	//Ö¡ÆğÊ¼±êÖ¾   
			{
				RS232_REC_Flag = 1;
			}
		}
		if(RS232_rec_counter > RS232_REC_BUFF_SIZE)//³¬¹ı½ÓÊÕ»º³åÇø´óĞ¡
		{
			RS232_rec_counter = 0;
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART1, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
    }	
}

/***********************************************************************
º¯ÊıÃû³Æ£ºRS232_Send_Data(unsigned char *send_buff,unsigned int length)
¹¦    ÄÜ£ºRS232·¢ËÍ×Ö·û´®
ÊäÈë²ÎÊı£ºsend_buff:´ı·¢ËÍµÄÊı¾İÖ¸Õë£»length£º·¢ËÍµÄÊı¾İ³¤¶È£¨×Ö·û¸öÊı£©
Êä³ö²ÎÊı£º
±àĞ´Ê±¼ä£º2012.11.22
±à Ğ´ ÈË£º
×¢    Òâ£º
***********************************************************************/
void RS232_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{			
		USART1->DR = send_buff[i];
		while((USART1->SR&0X40)==0);	
	}	
}

/***********************************************************************
º¯ÊıÃû³Æ£ºvoid RS485_Delay(uint32_t nCount)
¹¦    ÄÜ£ºRS485ÊÕ·¢ÑÓÊ±º¯Êı
ÊäÈë²ÎÊı£º
Êä³ö²ÎÊı£º
±àĞ´Ê±¼ä£º2012.11.22
±à Ğ´ ÈË£º
×¢    Òâ£º
***********************************************************************/
// static void RS485_Delay(uint32_t nCount)
// { 
//   while(nCount > 0)
//   { 
//   	  nCount --;   
//   }
// }
