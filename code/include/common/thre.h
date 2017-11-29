#ifndef __THREAD_COMM_H__
#define __THREAD_COMM_H__
#include <pthread.h>
#include <sched.h>

#define  SCHED_OTHER  0 //分时抢占式
#define  SCHED_FIFO  1 	//实时，先进先出
#define  SCHED_RR  2   	//实时

typedef enum
{
	SUCCESS = 0,
	FAILED = -1,
	PARAERR = -2,
}func_return_e;

typedef enum
{
	THRE_ALIVE = 0,
	THRE_UNEXIST = -1,
	ILLEGALITY_SIGN = -2,
}test_thre_e;

typedef  enum 
{
	EXIT = 0x00,
	RUN =0x01,
	SUSPEND=0x02,
}thre_ws_e;

typedef struct
{
	pthread_t ID;
	pthread_attr_t Attr;
	//pthread_mutex_t Mtx;
	//pthread_cond_t Cond;
	int Policy;
	int Priority;
	thre_ws_e WorkStat;
	test_thre_e Live;
	u32 PeriodTime;			//执行间隔时间，ms为单位
	void (*pfunc)(void *para);	/**/
}thread_s;

extern int test_pthread_alive(pthread_t tid, int *res);
//extern int get_pthread_schedpolicy(void);
extern int get_thread_policy (pthread_attr_t *attr, int *policy);
extern int set_thread_policy (pthread_attr_t *attr,int policy);

extern int get_threadpriority_scope(int policy,int *maxpriority, int *minpriority);
extern int get_thread_priority(pthread_attr_t *attr,int *priortity);
extern int set_thread_priority(pthread_attr_t *attr,int priority);
extern void thread_free(void* mtx);

#endif

