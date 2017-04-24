#include "main.h"

#define DEBUG

parallet_msg_t	*pblock = NULL;

void task_parallet(void *pdata)
{
	INT8U 			err;
	char i=0;
	pblock = (parallet_msg_t *)OSMemGet(g_hParalletRxMem, &err);
	if (err != OS_NO_ERR)
	{
		while(1)
		{
			p_err("%s err", __FUNCTION__);
			OSTimeDlyHMSM(0,0,1,0);
		}
	}
	else
	{
		memset(&pblock->u8Buf, 0, sizeof(pblock->u8Buf));
		pblock->len = 0;
		pblock->fst = 0;
		pblock->end = 0;
	}

	//while (1)
	//	OSTimeDly(1);
  
	while (1)
	{
		//OSSemPend(sem_udp_rec_flag,0,&err);
		// pblock->fst = 1;
		if (pblock->fst != 0)
		{
			pblock->end = os_time_get();
			if (pblock->end - pblock->fst > 1000)
			{
				OSQPost(hParalletMsgQueue, (void *)pblock);
				pblock = (parallet_msg_t *)OSMemGet(g_hParalletRxMem, &err);
				if (OS_NO_ERR == err)
				{
					memset(&pblock->u8Buf, 0, sizeof(pblock->u8Buf));
					pblock->len = 0;
					pblock->fst = 0;
					pblock->end = 0;
				}else{
					pblock=NULL;
					printf("there is no enugouh mem to stroe the priter dates\n");
				}
			}
		}		
		OSTimeDly(1);
	}
}


/*
void task_parallet(void *pdata)
{
	parallet_msg_t	*pblock = NULL;
	INT8U 			err;
	YJ_U16	t0;
	int i;
	int fst=0,end=0;

	p_info("%s comming", __FUNCTION__);

	pblock = (parallet_msg_t *)OSMemGet(g_hParalletRxMem, &err);
	if (err != OS_NO_ERR)
	{
		p_err("%s err", __FUNCTION__);
		while(1)
		{
			OSTimeDly(1);
		}
	}
	else
	{
		memset(&pblock->u8Buf, 0, sizeof(pblock->u8Buf));
		pblock->len = 0;
	}

	while(1)
	{
		t0 = parallel_read();
		if (!(t0 & 0x100))
		{
			pblock->u8Buf[pblock->len++] = t0;
			fst = os_time_get();
		}

		//end = os_time_get();
		if ((fst != 0) && (os_time_get() - fst > 1000))
		{
			OSQPost(hParalletMsgQueue, (void *)pblock);
			fst = 0;
			pblock = (parallet_msg_t *)OSMemGet(g_hParalletRxMem, &err);
			if (OS_NO_ERR == err)
			{
				memset(&pblock->u8Buf, 0, sizeof(pblock->u8Buf));
				pblock->len = 0;
			}
		}
		sleep(0);
	}
}
*/


