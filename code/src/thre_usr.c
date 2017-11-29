#include "include.h"

thread_s gthre[MAX_THRE_TOTAL];
extern char gledip[];
int reminspace=0;
extern int gRemin;
int up2usr(int total,int remain)
{
    int buf[128];
    int tmpbuf[32];
    int j=-1;
    int fd;
    memset(buf,0,128);
    j= tcp_blkConn(SysCfg.UsrCommn.IP, SysCfg.UsrCommn.MsgPort, &fd);
    if(j<0)
    {
        writelog(ZLOG_ERR, "%s:%d ���Ӳ���",SysCfg.UsrCommn.IP,SysCfg.UsrCommn.MsgPort);
        return -1;
    }
    
    strcpy(buf,"Linux3352;");
    strcat(buf,"172.16.42.74;");
    memset(tmpbuf,0,sizeof(tmpbuf));
    
    sprintf(tmpbuf,"%d",total);	
    strcat(buf,tmpbuf);
    strcat(buf,";");
    
    memset(tmpbuf,0,sizeof(tmpbuf));
    sprintf(tmpbuf,"%d",remain);
    strcat(buf,tmpbuf);    
   // strcat(buf,";");
    //printf("----------------------------\r\n");
   usleep(50000);
    j = send(fd,buf,strlen(buf),0);
    if( j != strlen(buf))/* ���Ͳɼ���ϸָ���ǵİ�������20 */
    {
    	writelog(ZLOG_ERR,"ʣ�೵λ���͵�������(%s:%d)--%s--ʧ��",SysCfg.UsrCommn.IP,SysCfg.UsrCommn.MsgPort,buf);
    	tcp_blkClose(&fd);
    	return -1;
    }
    
    writelog(ZLOG_SYS,"ʣ�೵λ���͵�������(%s:%d)--%s--�ɹ�", SysCfg.UsrCommn.IP, SysCfg.UsrCommn.MsgPort,buf);
    tcp_blkClose(&fd);
    return 0;

}



/*******************************************************************************
* ������  : func_clt_thre 
* ����	  : �ɼ��̺߳���
* ����	  : para:�̶߳���
*         : 
* ����	  : ��

*******************************************************************************/
void func_clt_thre(void *para)
{
	thread_s *p;
	p = (thread_s *)para;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);	/* ��������ȡ�� */
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);	/* ����һ��ȡ����ȡ�� */
	
	for(; p->WorkStat == RUN; )
	{
		pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);/* ��ֹ�߳��쳣�˳� */
		pthread_mutex_lock(&gwlist->Mtx);
		//pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);/*block-->unlock-->wait()return-->lock*/

		/* �ɼ���λ��Ϣ */
		get_spaceInfo2list(gwlist);
		
		pthread_mutex_unlock(&gwlist->Mtx);
		pthread_cond_signal(&gwlist->Cond);
		pthread_cleanup_pop(0);	
		usleep(p->PeriodTime*1000);
	}
	printf("func_clt_thre was exit\r\n");
	pthread_exit(NULL);
	
}



/*******************************************************************************
* ������  : func_proc_thre 
* ����	  : �����̺߳���
* ����	  : para:�̶߳���
*         : 
* ����	  : ��

*******************************************************************************/
void func_proc_thre(void *para)
{
	thread_s *p;
	p = (thread_s *)para;

	int spacttl=0;//����
	int spacrmn=0;//ʣ�೵λ
	int incnt_incar=0;//���ó�λ��
	int abnttl;
	char buf[8];

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);	/* ��������ȡ�� */
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);	/* ����һ��ȡ����ȡ�� */
	
	for(; p->WorkStat == RUN; )
	{
		pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);/* ��ֹ�߳��쳣�˳� */
		/* �������� */
		pthread_mutex_lock(&gwlist->Mtx);
		pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);/*block-->unlock-->wait()return-->lock*/
		
		//����Ϣ��
		pthread_mutex_lock(&msglist->Mtx);
		//pthread_cond_wait(&msglist->Cond, &msglist->Mtx);

		crtmsg(gwlist, msglist, &spacttl, &spacrmn ,&incnt_incar,&abnttl);
		//����Ϣ��
		pthread_mutex_unlock(&msglist->Mtx);
		pthread_cond_signal(&msglist->Cond);
		//���³�λ��Ϣ�����ݿ�
		update_spaceinfo2db(gwlist);
		

		//��������
		pthread_mutex_unlock(&gwlist->Mtx);
		pthread_cond_signal(&gwlist->Cond);
		pthread_cleanup_pop(0);

		
		//LED��ʾ���ʺ����������ʱ��ʹ�ò����ݿ�ķ�ʽ
		memset(buf,0,sizeof(buf));
             gRemin = spacrmn;
             /*
             int k=0;
             if(spacrmn >= SysCfg.SpaceFull_RealRmn) k = spacrmn -SysCfg.SpaceFull_RealRmn;
             else k=0;
             reminspace = k;
		sprintf(buf,"%d",k);
        
             //sprintf(buf,"%d",spacrmn);	
		//gledip;
		//setLED_num("210.39.2.142",5005,buf);
		setLED_num(gledip,5005,buf);
		*/
            //_shl_20161225_��Ϣ�۶��ƻ�����
             up2usr(spacttl,spacrmn);
		
	}
	printf("func_proc_thre was exit\r\n");
	pthread_exit(NULL);
	
}

/*******************************************************************************
* ������  : func_proc_thre 
* ����	  : �����̺߳���
* ����	  : para:�̶߳���
*         : 
* ����	  : ��

*******************************************************************************/
void func_msg_thre(void *para)
{
	thread_s *p;
	p = (thread_s *)para;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);	/* ��������ȡ�� */
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);	/* ����һ��ȡ����ȡ�� */
	
	for(; p->WorkStat == RUN; )
	{
		//usleep(p->PeriodTime*1000);
		
		pthread_cleanup_push(thread_free,(void*)&msglist->Mtx);/* ��ֹ�߳��쳣�˳� */
		pthread_mutex_lock(&msglist->Mtx);
		pthread_cond_wait(&msglist->Cond, &msglist->Mtx);		/*block-->unlock-->wait()return-->lock*/

		msg2mng(msglist, SysCfg.Mng.IP, SysCfg.Mng.MsgPort);
		
		pthread_mutex_unlock(&msglist->Mtx);
		pthread_cond_signal(&msglist->Cond);
		pthread_cleanup_pop(0);
	}
	printf("func_proc_thre was exit\r\n");
	pthread_exit(NULL);
	
}


/*******************************************************************************
* ������  : func_proc_thre 
* ����	  : �����̺߳���
* ����	  : para:�̶߳���
*         : 
* ����	  : ��

*******************************************************************************/
void func_gpio_thre(void *para)
{
	thread_s *p;
	p = (thread_s *)para;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);	/* ��������ȡ�� */
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);	/* ����һ��ȡ����ȡ�� */
	
	for(; p->WorkStat == RUN; )
	{
		usleep(p->PeriodTime*1000);
		readgpi_machupcfg();
		//if(reminspace > 0)
            gpiedge_proc();
		wrgpo_machupcfg();
	}
	printf("func_proc_thre was exit\r\n");
	pthread_exit(NULL);
	
}

/*******************************************************************************
* ������  : thre_init 
* ����	  : �̳߳�ʼ��
* ����	  : ��
* ����	  : SUCCESS,FAILED
*******************************************************************************/
int thre_init(void)
{
	
	gthre[CLT_THRE].WorkStat = RUN;
	gthre[CLT_THRE].PeriodTime = 10000;	

	gthre[CLT_THRE].Policy = SCHED_OTHER;
	gthre[CLT_THRE].Priority = 22;
	gthre[CLT_THRE].pfunc = func_clt_thre;

	gthre[PROC_THRE].WorkStat = RUN;
	gthre[PROC_THRE].PeriodTime = 500;	

	gthre[PROC_THRE].Policy = SCHED_OTHER;
	gthre[PROC_THRE].Priority = 23;
	gthre[PROC_THRE].pfunc = func_proc_thre;
	
	gthre[MSG_THRE].WorkStat = RUN;
	gthre[MSG_THRE].PeriodTime = 500;
	gthre[MSG_THRE].Policy = SCHED_OTHER;
	gthre[MSG_THRE].Priority = 23;
	gthre[MSG_THRE].pfunc = func_msg_thre;


	gthre[GPIO_THRE].WorkStat = RUN;
	gthre[GPIO_THRE].PeriodTime = 30;
	gthre[GPIO_THRE].Policy = SCHED_OTHER;
	gthre[GPIO_THRE].Priority = 23;
	gthre[GPIO_THRE].pfunc = func_gpio_thre;
	

	int i=0;//MAX_THRE_TOTAL
	for(i=0;i<MAX_THRE_TOTAL;i++)
	{
		int ret;
		ret = pthread_attr_init(&gthre[i].Attr);
		if(ret != 0)
		{
			writelog(ZLOG_ERR, "pthread_attr_init: thread=%d error\r\n", i);
			return -1;
	    }

		ret = set_thread_policy(&gthre[i].Attr,gthre[i].Policy);
		if(ret != 0)
		{
			writelog(ZLOG_ERR, "set_thread_policy: thread=%d error\r\n", i);
			return -1;
		}
		
		if(gthre[i].Policy != SCHED_OTHER)
		{
			ret = set_thread_priority(&gthre[i].Attr,gthre[i].Priority);	
			if(ret != 0)
			{
				writelog(ZLOG_ERR, "set_thread_priority: thread=%d error\r\n", i);
				return -1;
			}
		}

		ret=pthread_create(&gthre[i].ID, &gthre[i].Attr, (void*)gthre[i].pfunc, (void*)&gthre[i]);
		if(ret!=0)
		{
			writelog(ZLOG_ERR, "pthread_create:thread=%d error\r\n",i);
			return -1;
		}

	}
	
	return 0;
}
/*******************************************************************************
* ������  : test_thres 
* ����	  : �����̵߳�ǰ������״̬
* ����	  : ��
* ����	  : SUCCESS,FAILED
*******************************************************************************/
int test_thres(void)
{
	int i=0;
	for(i=0;i<MAX_THRE_TOTAL;i++)
	{
		test_pthread_alive(gthre[i].ID,&gthre[i].Live);
		if(gthre[i].Live != THRE_ALIVE)
		{
			writelog(ZLOG_ERR, "test_pthread_alive: thread=%d error\r\n", i);
			return -1;
		}
	}
	return 0;
}











#if 0
	int res;
	get_thread_policy(&threadAttr_test0,&res);
	printf("policy0:%d\r\n",res);


	int maxPrio;
	int minPrio;
	get_threadpriority_scope(SCHED_OTHER, &maxPrio, &minPrio);
	printf("priority_scope0:%d,%d\r\n",maxPrio,minPrio);


	int priority;
	get_thread_priority(&threadAttr_test0,&priority);
#endif


