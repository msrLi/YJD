#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__


/* �������ͷ�ļ� */
#include "app_cfg.h"
#include "ucos_ii.h"

/* �ṩ��׼������� */
#define LWIP_PROVIDE_ERRNO

typedef OS_EVENT*      HANDLER;

/* ������������޹ص��������� */


/* ����ָ������ */
typedef u32_t mem_ptr_t;


/* �ṹ�� */
#define PACK_STRUCT_FIELD(x)  x 
#define PACK_STRUCT_STRUCT	
#define PACK_STRUCT_BEGIN 	 
#define PACK_STRUCT_END


/* ƽ̨��������Ͷ��� */
#if 1

/* ������ڸ�ʽ�����Ҫ�Լ�ʵ�� */
//extern void printf(char *fmt,...);
#include "stdio.h"

#define lwip_printf printf

#define LWIP_PLATFORM_DIAG(x)	 { lwip_printf x; }
#define LWIP_PLATFORM_ASSERT(x)  { lwip_printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); while(1); }

#define LWIP_PLATFORM_ERR(x)  { lwip_printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__);}
#else

/* ����㲻��ʵ������Ĵ��ڸ�ʽ�����, ��ô #if ��Ӧ��Ϊ 0, ���µĽ������� */

#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)  { while(1); }

#endif


/* �ٽ����ı���( ʹ��uCOS-II�ĵ������ٽ紦��ʽ ) */
#define SYS_ARCH_DECL_PROTECT(x) u32_t cpu_sr
#define SYS_ARCH_PROTECT(x)      cpu_sr = local_irq_save()
#define SYS_ARCH_UNPROTECT(x)    local_irq_restore(cpu_sr)


#endif /* __ARCH_CC_H__ */


