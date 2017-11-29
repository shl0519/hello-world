#include "include.h"

/*******************************************************************************
* ������  : init_gwlist
* ����		: ��ʼ��������Ϣ����
* ����		: ��
* ���		:
* ����		: ��ʼ����Ķ��й���
*******************************************************************************/
gw_list_s* init_gwlist(void)/*����ָ��Q*/
{
	gw_list_s *q;
	if((q =(gw_list_s *)malloc(sizeof(gw_list_s)))==NULL)
	{
		writelog(ZLOG_ERR, "������ڽڵ�ռ�����˴��˳�!!");
		return NULL;
	}
	memset(q,0,sizeof(gw_list_s));
	q->Head=NULL;		 /*ָ��ͷָ��p*/
	q->Tail=NULL;		 /*�����ն���*/
	q->Len=0;			 /*���г�Ա����ͳ��*/
	//q->Mtx = PTHREAD_MUTEX_INITIALIZER;
	//q->Cond = PTHREAD_COND_INITIALIZER;

	q->ParkId=0;
	q->GarageId=0;
	
	pthread_mutex_init(&(q->Mtx), NULL);
	pthread_cond_init(&(q->Cond), NULL);
	
	writelog(ZLOG_SYS, "����������Ϣ���гɹ�.");
	return q;
}

/*******************************************************************************
* ������  : ins_gwlist
* ����	  : ������нڵ�
* ����	  : *q:����ָ��
			*n:��Ԫ�ڵ�
* ���	  : ��
* ����	  : 0:����
			-1:����
*******************************************************************************/
int ins_gwlist(gw_list_s *q, int gwId)
{
	if(q == NULL)
	{
		writelog(ZLOG_ERR, "[ins_gwlist] Invalid argument!");
		return -1;		
	}
	
	if(q->Len >= MAX_GWLIST_LEN)
	{
		writelog(ZLOG_ERR, "�����޶���������޶�����");
		return -1;	
	}
	
	gw_node_s *n;
	if((n =(gw_node_s *)malloc(sizeof(gw_node_s)))==NULL)
	{
		writelog(ZLOG_ERR, "�������ض����ڴ�������");
		return -1;
	}
	
	memset(n,0,sizeof(gw_node_s));
	n->gwID = gwId;
	n->DevErr = GW_COMMU_ERR;			   /* ��ʼ��Ĭ�������豸��û�����ӵģ����нڵ���û�����ӵ� */
	n->AllSpaceDevErr = DETOR_COMMU_ERR;
	n->Next = NULL; 		   /* n����һ��Ϊ�� */
	
	if(q->Tail != NULL)
	{
		q->Tail->Next = n;
	}
	q->Tail = n;		   		/* ��β�ڵ�ָ��n */
	if(q->Head == NULL)
	{
		q->Head = n;
	}
	q->Len = q->Len + 1;		/* ���ȼ�һ */
	return 0;
}


#if 0
int ins_gwlist(gw_list_s *q, gw_node_s *n)
{
	if((q == NULL)||(n == NULL))
	{
		writelog(ZLOG_ERR, "[ins_gwlist] Invalid argument!");
		return -1;		
	}
	
	if(q->Len >= MAX_GWLIST_LEN)
	{
		writelog(ZLOG_ERR, "�����޶���������޶�����");
		return -1;	
	}
	n->Next = NULL; 		   /* n����һ��Ϊ�� */
	if(q->Tail != NULL)
	{
		q->Tail->Next = n;
	}
	q->Tail = n;		   		/* ��β�ڵ�ָ��n */
	if(q->Head == NULL)
	{
		q->Head = n;
	}
	q->Len = q->Len + 1;		/* ���ȼ�һ */
	return 0;
}
#endif

