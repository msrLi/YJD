#include "api.h"
#include "debug.h"
#include "wait.h"
/*
注：初始化OSSemCreate(1)，那么一个任务中有OSSemPend，
那么可以执行，执行之后cnt==0，其他任务的OSSemPend无法获得sem，
只能等待，除非任务一有OSSemPost，使其cnt++，这样其他任务的Pend可以执行。
OSSemCreate(0)为一个等待事件
// 	sem_RS232_rec_flag = OSSemCreate(1); //创建一个信号量,
*/
wait_event_t init_event()
{

	OS_EVENT *event = OSSemCreate(0);
	if (!event)
		p_err("init_waitqueue_head err");

	return (wait_event_t)event;
}

int wait_event(wait_event_t wq)
{
	INT8U perr;
	if (wq == 0)
	{
		return  - 1;
	}
	OSSemPend(wq, 0, &perr);//持续等待wq信号量有效
	if (perr == OS_ERR_NONE)
	{
		return 0;
	}
	p_err("add_wait_queue err:%d\n", perr);
	return  - 1;
}

int wait_event_timeout(wait_event_t wq, unsigned int timeout)
{
	INT8U perr;
	uint32_t ticks;
	if (wq == 0)
	{
		p_err("wait_event_timeout wq err");
		return  - 1;
	}
	ticks = (timeout *OS_TICKS_PER_SEC) / 1000;
	OSSemPend(wq, ticks, &perr);
	if (perr == OS_ERR_NONE)
		return 0;
	else if (perr == OS_ERR_TIMEOUT){
		return WAIT_EVENT_TIMEOUT;
	}
	return  - 1;

}

void wake_up(wait_event_t wq)
{
	INT8U perr = OSSemPost(wq);
	if (perr != OS_ERR_NONE)
		p_err("wake_up err %d", perr);
}

void del_event(wait_event_t wq)
{
	INT8U err;
	OSSemDel (wq, OS_DEL_ALWAYS, &err);
}


void clear_wait_event(wait_event_t wq)
{
	INT8U perr;

	if (wq == 0)
	{
		p_err("clear_wait_event wq err");
		return ;
	}
	OSSemSet(wq, 0, &perr);
	if (perr != OS_ERR_NONE)
		p_err("clear_wait_event err %d", perr);

	return ;
}
