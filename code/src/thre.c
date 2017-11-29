#include "include.h"


/*******************************************************************************
* ������  : �����߳��Ƿ���
* ����	  : pthread_kill�ķ���ֵ���ɹ���0�� �̲߳����ڣ�ESRCH�� �źŲ��Ϸ���EINVAL��
* ����	  : void *arg : ��Σ�ʱ��
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
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
* ������  : get_thread_policy 
* ����	  : ��ȡ�̵߳ĵ��Ȳ���
* ����	  : *attr:�߳�����ָ��
* ���	  : *policy :���Ȳ���
* ����	  : SUCCESS,FAILED
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
* ������  : set_thread_policy 
* ����	  : �����̵߳ĵ��Ȳ���
* ����	  : *attr:�߳�����ָ��
* 		  : policy :���Ȳ���
* ����	  : SUCCESS,FAILED
	
//���У���sched.h �жԵ��Ȳ��Զ������£� 
#define  SCHED_OTHER  0 //��ʱ
#define  SCHED_FIFO  1 //ʵʱ���Ƚ��ȳ�,��ռ
#define  SCHED_RR  2   //ʵʱ

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
* ������  : get_threadpriority_scope 
* ����	  : ��ȡ�̵߳��Ȳ��Ե����ȼ�ȡֵ��Χ
* ����	  : policy :���Ȳ���
* ����	  : SUCCESS,FAILED
	
//���У���sched.h �жԵ��Ȳ��Զ������£� 
#define  SCHED_OTHER  0 //��ʱ��ռʽ
#define  SCHED_FIFO  1 //ʵʱ���Ƚ��ȳ�
#define  SCHED_RR  2   //ʵʱ

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
* ������  : get_thread_priority 
* ����	  : �����̵߳����ȼ�
* ����	  : attr:�߳�����
* ���    : para :���Ȳ���
* ����	  : SUCCESS,FAILED
	
//���У���sched.h �жԵ��Ȳ��Զ������£� 
#define  SCHED_OTHER  0 //��ʱ��ռʽ
#define  SCHED_FIFO  1 //ʵʱ���Ƚ��ȳ�
#define  SCHED_RR  2   //ʵʱ

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
* ������  : set_thread_priority 
* ����	  : �����̵߳����ȼ�
* ����	  : attr:�߳�����
*         : policy :���Ȳ���
* ����	  : SUCCESS,FAILED
	
//���У���sched.h �жԵ��Ȳ��Զ������£� 
#define  SCHED_OTHER  0 //��ʱ��ռʽ
#define  SCHED_FIFO  1 //ʵʱ���Ƚ��ȳ�
#define  SCHED_RR  2   //ʵʱ
˵��������������Ը��캯����������������������������ֵ���ڱ��������ڿ��Խ���һ��
struct sched_param�ľֲ�������Ȼ������ȼ�����para->__sched_priority���������Լ���
ȫ�ֱ�������
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
* ������  : thread_free 
* ����	  : �߳��쳣��ֹʱ����Ҫ�ͷ���Դ
* ����	  : SUCCESS,FAILED
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
* ������  : get_pthread_schedpolicy 
* ����	  : ��ȡ�̵߳ĵ��Ȳ���
* ����	  : ��
* ���	  : ��
* ����	  : SUCCESS,FAILED
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

    ret = pthread_attr_getschedpolicy(&attr, &policy);//��ȡ�̵߳��Ȳ���
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

//ʡ�����̻߳������Լ����������ĳ�ʼ��
//�̹߳�������sec���ȡ�߳���Ϣ
//���������ֱ�Ϊ���߳���Ϣ���߳�ID����ʱ����
bool ManagePthread_TimeReadSignal(PTHREAD_BUF *rbuf, PTHREAD_ID thread_num, int sec)
{
    bool b_valid = false;
    struct timespec to;
    int err;
    to.tv_sec = time(NULL) + sec;
    to.tv_nsec = 0;

 //����
    pthread_mutex_lock(&managePthread.g_pthread_mutex[thread_num]);
 //��ʱsec�������ȴ�������select
    err = pthread_cond_timedwait(&managePthread.g_pthread_cond[thread_num], &managePthread.g_pthread_mutex[thread_num], &to);
    if(err == ETIMEDOUT)
    {
        pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
        return false;
    }

 //��ȡ�߳���Ϣ
    if(managePthread.g_pthread_info[thread_num] == WRITE_FLAG)
    {
        managePthread.g_pthread_info[thread_num] = READ_FLAG;
        memcpy((PTHREAD_BUF *)rbuf, (PTHREAD_BUF *)&managePthread.g_pthread_buf[thread_num], sizeof(PTHREAD_BUF));
        b_valid = true;
    }

 //����
    pthread_mutex_unlock(&managePthread.g_pthread_mutex[thread_num]);
    return b_valid;
}
//������ȡ�߳���Ϣ
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
//����/�����߳���Ϣ
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
//�㲥
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
















