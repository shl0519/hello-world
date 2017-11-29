#include "include.h"


/*******************************************************************************
* 函数名  : 测试线程是否存活
* 功能	  : pthread_kill的返回值：成功（0） 线程不存在（ESRCH） 信号不合法（EINVAL）
* 输入	  : void *arg : 入参，时间
* 输出	  : 无
* 返回	  : 0:正常
			-1:初始化内存错误
*******************************************************************************/

int test_pthread_alive(pthread_t tid, int *res)
{
	if((tid == 0)||(res == NULL))
	{
		writelog(ZLOG_ERR, "[test_pthread_alive] Invalid argument!");
		return PARAERR;
	}
	
	int ret;
	ret = pthread_kill(tid,0);
	if(ret == ESRCH)
	{
		//writelog(ZLOG_ERR, "[test_pthread_alive]The thread ID=%d was unexist or was return\r\n",(int)tid);
		*res =  THRE_UNEXIST;
	}
	else if(ret == EINVAL)
	{
		//writelog(ZLOG_ERR, "[test_pthread_alive] Illegality sign\r\n");
		*res =  ILLEGALITY_SIGN;
	}
	else
	{
		*res =  THRE_ALIVE;
	}
	return SUCCESS;
}

/*******************************************************************************
* 函数名  : get_thread_policy 
* 功能	  : 获取线程的调度策略
* 输入	  : *attr:线程属性指针
* 输出	  : *policy :调度策略
* 返回	  : SUCCESS,FAILED
*******************************************************************************/
int get_thread_policy (pthread_attr_t *attr, int *policy)
{
	if((attr == NULL) || (policy == NULL))
	{
		writelog(ZLOG_ERR, "[get_thread_policy] Invalid argument!");
		return PARAERR;
	}
	
	int ret;	
    ret = pthread_attr_getschedpolicy(attr, policy);
    if(ret != 0)
	{
		writelog(ZLOG_ERR, "[get_thread_policy] error!");
		return FAILED;
    }

    switch(*policy)
	{
        case SCHED_FIFO:
		{
			writelog(ZLOG_SYS, "[get_thread_policy] SCHED_FIFO");
			break;
        }
        case SCHED_RR:
		{
			writelog(ZLOG_SYS, "[get_thread_policy] SCHED_RR");
			break;
        }
        case SCHED_OTHER:
		{
			writelog(ZLOG_SYS, "[get_thread_policy] SCHED_OTHER");
	        break;
        }
        default:
		{
			writelog(ZLOG_SYS, "[get_thread_policy] default");
            break;
		}
    }
    return SUCCESS;
}
/*******************************************************************************
* 函数名  : set_thread_policy 
* 功能	  : 设置线程的调度策略
* 输入	  : *attr:线程属性指针
* 		  : policy :调度策略
* 返回	  : SUCCESS,FAILED
	
//其中，在sched.h 中对调度策略定义如下： 
#define  SCHED_OTHER  0 //分时
#define  SCHED_FIFO  1 //实时，先进先出,抢占
#define  SCHED_RR  2   //实时

*******************************************************************************/
int set_thread_policy (pthread_attr_t *attr,int policy)
{
	if((attr == NULL) || (policy < 0) || (policy > 2))
	{
		writelog(ZLOG_ERR, "[set_thread_policy] Invalid argument!");
		return PARAERR;
	}
	int ret;
	ret = pthread_attr_setschedpolicy(attr, policy);
    if(ret != 0)
	{
		writelog(ZLOG_ERR, "[set_thread_policy] error!");
		return FAILED;
    }
	return SUCCESS;
}
/*******************************************************************************
* 函数名  : get_threadpriority_scope 
* 功能	  : 获取线程调度策略的优先级取值范围
* 输入	  : policy :调度策略
* 返回	  : SUCCESS,FAILED
	
//其中，在sched.h 中对调度策略定义如下： 
#define  SCHED_OTHER  0 //分时抢占式
#define  SCHED_FIFO  1 //实时，先进先出
#define  SCHED_RR  2   //实时

*******************************************************************************/
int get_threadpriority_scope(int policy,int *maxpriority, int *minpriority)
{
	if((policy < 0) || (policy > 2))
	{
		writelog(ZLOG_ERR, "[get_threadpriority_scope] Invalid argument!");
		return PARAERR;
	}
    int priority;
	priority = sched_get_priority_max(policy);
    if(priority < 0)
    {
		writelog(ZLOG_ERR, "[get_threadpriority_scope] sched_get_priority_max error!");
		return FAILED;

	}
	*maxpriority = priority;
	writelog(ZLOG_SYS, "max_priority = %d", priority);
	priority = sched_get_priority_min(policy);
    if(priority < 0)
    {
		writelog(ZLOG_ERR, "[get_threadpriority_scope] sched_get_priority_min error!");
		return FAILED;
	}
	writelog(ZLOG_SYS, "min_priority = %d\n", priority);
	*minpriority = priority;
	return SUCCESS;
}
/*******************************************************************************
* 函数名  : get_thread_priority 
* 功能	  : 设置线程的优先级
* 输入	  : attr:线程属性
* 输出    : para :调度策略
* 返回	  : SUCCESS,FAILED
	
//其中，在sched.h 中对调度策略定义如下： 
#define  SCHED_OTHER  0 //分时抢占式
#define  SCHED_FIFO  1 //实时，先进先出
#define  SCHED_RR  2   //实时

*******************************************************************************/
int get_thread_priority(pthread_attr_t *attr,int *priortity)
{
	if((attr == NULL) || (priortity == NULL))
	{
		writelog(ZLOG_ERR, "[get_thread_priority] Invalid argument!");
		return PARAERR;
	}
	
    int ret;
	struct sched_param para;
	ret = pthread_attr_getschedparam (attr, &para);
    if(ret != 0)
    {
		writelog(ZLOG_ERR, "[pthread_attr_getschedparam] error!");
		return FAILED;
	}
	writelog(ZLOG_SYS, "priority = %d\n", para.__sched_priority);
	*priortity = para.__sched_priority;
	return SUCCESS;
}

/*******************************************************************************
* 函数名  : set_thread_priority 
* 功能	  : 设置线程的优先级
* 输入	  : attr:线程属性
*         : policy :调度策略
* 返回	  : SUCCESS,FAILED
	
//其中，在sched.h 中对调度策略定义如下： 
#define  SCHED_OTHER  0 //分时抢占式
#define  SCHED_FIFO  1 //实时，先进先出
#define  SCHED_RR  2   //实时
说明，在这里面可以改造函数的输入参数，输入参数可以是数值，在本函数体内可以建立一个
struct sched_param的局部变量，然后把优先级赋给para->__sched_priority，这样可以减少
全局变量设置
*******************************************************************************/
int set_thread_priority(pthread_attr_t *attr,int priority)
{
	if(attr == NULL)
	{
		writelog(ZLOG_ERR, "[set_thread_priority] Invalid argument!");
		return PARAERR;
	}
	struct sched_param para;
	para.__sched_priority = priority;
    int ret;
	ret = pthread_attr_setschedparam(attr, &para);
    if(ret != 0)
    {
		writelog(ZLOG_ERR, "[set_thread_priority] error!");
		return FAILED;
	}
	return SUCCESS;
}

/*******************************************************************************
* 函数名  : thread_free 
* 功能	  : 线程异常终止时，需要释放资源
* 返回	  : SUCCESS,FAILED
*******************************************************************************/

void thread_free(void* mtx) 
{
    (void)pthread_mutex_unlock((pthread_mutex_t*) mtx);
}
#if 0
void thread_free(void*arg) 
{
    printf("Clean up handler of second thread.\n");
    free(arg);
    (void)pthread_mutex_unlock(&mtx);
}
#endif

















#if 0
/*******************************************************************************
* 函数名  : get_pthread_schedpolicy 
* 功能	  : 获取线程的调度策略
* 输入	  : 无
* 输出	  : 无
* 返回	  : SUCCESS,FAILED
*******************************************************************************/
int get_pthread_schedpolicy(void)
{
    pthread_attr_t attr;
    int ret = 0; 
	int policy = 0;

    ret = pthread_attr_init(&attr);
    if(ret != 0)
	{
		printf("pthread_attr_init err,errcode is %d\r\n",ret);
		return FAILED;
    }

    ret = pthread_attr_getschedpolicy(&attr, &policy);//获取线程调度策略
    if(ret != 0)
	{
		printf("pthread_attr_getschedpolicy error");
		return FAILED;
    }

    switch(policy)
	{
        case SCHED_FIFO:
		{
			printf("SCHED_FIFO\n");
			break;
        }
        case SCHED_RR:
		{
			printf("SCHED_RR\n");
			break;
        }
        case SCHED_OTHER:
		{
	        printf("SCHED_OTHER\n");
	        break;
        }
        default:
		{
            printf("default\n");
            break;
		}
    }
	pthread_attr_destroy(&attr);
    return SUCCESS;
}


#endif



#if 0

//省略了线程互斥量以及条件变量的初始化
//线程管理：阻塞sec秒读取线程信息
//三个参数分别为：线程信息、线程ID、超时秒数
bool ManagePthread_TimeReadSignal(PTHREAD_BUF *rbuf, PTHREAD_ID thread_num, int sec)
{
    bool b_valid = false;
    struct timespec to;
    int err;
    to.tv_sec = time(NULL) + sec;
    to.tv_nsec = 0;

 //上锁
    pthread_mutex_lock(&managePthread.g_pthread_mutex[thread_num]);
 //超时sec秒阻塞等待，类似select
    err = pthread_cond_timedwait(&managePthread.g_pthread_cond[thread_num], &managePthread.g_pthread_mutex[thread_num], &to);
    if(err == ETIMEDOUT)
    {
        pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
        return false;
    }

 //获取线程信息
    if(managePthread.g_pthread_info[thread_num] == WRITE_FLAG)
    {
        managePthread.g_pthread_info[thread_num] = READ_FLAG;
        memcpy((PTHREAD_BUF *)rbuf, (PTHREAD_BUF *)&managePthread.g_pthread_buf[thread_num], sizeof(PTHREAD_BUF));
        b_valid = true;
    }

 //解锁
    pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
    return b_valid;
}
//阻塞读取线程信息
bool ManagePthread_ReadSignal(PTHREAD_BUF *rbuf, PTHREAD_ID thread_num, bool wait)
{
    bool b_valid = false;
    pthread_mutex_lock(&managePthread.g_pthread_mutex[thread_num]);
    if(wait == true)
        pthread_cond_wait(&managePthread.g_pthread_cond[thread_num], &managePthread.g_pthread_mutex[thread_num]);
    if(managePthread.g_pthread_info[thread_num] == WRITE_FLAG)
    {
        managePthread.g_pthread_info[thread_num] = READ_FLAG;
        memcpy((PTHREAD_BUF *)rbuf, (PTHREAD_BUF *)&managePthread.g_pthread_buf[thread_num], sizeof(PTHREAD_BUF));
        b_valid = true;
    }
    pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
    return b_valid;
}
//激活/发送线程信息
bool ManagePthread_SendSignal(PTHREAD_BUF *sbuf, PTHREAD_ID thread_num)
{
    bool b_valid = false;
    pthread_mutex_lock(&managePthread.g_pthread_mutex[thread_num]);
    managePthread.g_pthread_info[thread_num] = WRITE_FLAG;
    if(sbuf)
    {
        memcpy((PTHREAD_BUF *)&managePthread.g_pthread_buf[thread_num], (PTHREAD_BUF *)sbuf, sizeof(PTHREAD_BUF));
    }
    pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
    pthread_cond_signal(&managePthread.g_pthread_cond[thread_num]);
    b_valid = true;
    return b_valid;
}
//广播
bool ManagePthread_BroadcastSignal(PTHREAD_BUF *sbuf, PTHREAD_ID thread_num)
{
    bool b_valid = false;
    pthread_mutex_lock(&managePthread.g_pthread_mutex[thread_num]);
    managePthread.g_pthread_info[thread_num] = WRITE_FLAG;
    memcpy((PTHREAD_BUF *)&managePthread.g_pthread_buf[thread_num], (PTHREAD_BUF *)sbuf, sizeof(PTHREAD_BUF));
    pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
    pthread_cond_broadcast(&managePthread.g_pthread_cond[thread_num]);
    b_valid = true;
    return b_valid;
}

#endif
















