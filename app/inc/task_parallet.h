#ifndef __YJ_APP_INC_TASK_PARALLET_H_
#define __YJ_APP_INC_TASK_PARALLET_H_

#define 	BUF_SIZE	(1024*2u)

typedef struct _parallet_msg_t_
{
	INT16U	len;
	INT16U	fst;
	INT16U	end;
	INT8U	u8Buf[BUF_SIZE];
}parallet_msg_t, *parallet_msg_p;

extern parallet_msg_t	*pblock;

#ifdef __cplusplus
extern "C"
{
#endif

void task_parallet(void *pdata);

#ifdef __cplusplus
}
#endif
#endif

