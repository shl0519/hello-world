#include "include.h"

/*******************************************************************************
* ������  : init_msglist
* ����		: ��ʼ��������Ϣ����
* ����		: ��
* ���		:
* ����		: ��ʼ����Ķ��й���
*******************************************************************************/
msg_list_s* init_msglist(void)/*����ָ��Q*/
{
	msg_list_s *q;
	if((q =(msg_list_s *)malloc(sizeof(msg_list_s)))==NULL)
	{
		writelog(ZLOG_ERR, "������ڽڵ�ռ�����˴��˳�!!");
		return NULL;
	}
	memset(q,0,sizeof(msg_list_s));
	q->Head=NULL;		 /*ָ��ͷָ��p*/
	q->Tail=NULL;		 /*�����ն���*/
	q->Len=0;			 /*���г�Ա����ͳ��*/
	//q->Mtx = PTHREAD_MUTEX_INITIALIZER;
	//q->Cond = PTHREAD_COND_INITIALIZER;

	pthread_mutex_init(&(q->Mtx), NULL);
	pthread_cond_init(&(q->Cond), NULL);
	
	writelog(ZLOG_SYS, "�����ϴ���Ϣ���гɹ�.");
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
int ins_msglist(msg_list_s *q, char *msg, int msglen)
{
	if((q == NULL)||(msg == NULL)||(msglen == 0))
	{
		writelog(ZLOG_ERR, "[ins_msglist] Invalid argument!");
		return -1;		
	}
	
	if(q->Len >= MAX_MSGLIST_LEN)
	{
		writelog(ZLOG_ERR, "�����޶���Ϣ��������޶�����");
		return -1;	
	}
	
	msg_node_s *n;
	if((n =(msg_node_s *)malloc(sizeof(msg_node_s)))==NULL)
	{
		writelog(ZLOG_ERR, "������Ϣ�����ڴ�������");
		return -1;
	}
	
	memset(n,0,sizeof(msg_node_s));
	n->MsgLen = msglen;
	memcpy(n->Msg, msg, msglen);
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


/*******************************************************************************
* ������  : judgeNull_SqlopQueue
* ����	  : �ж����ݿ���������е��Ƿ�Ϊ��
* ����	  : stru_sqlopq *q:���ݿ����������

* ���	  :
* ����	  : 0:��Ϊ��
			-1:Ϊ��״̬
*******************************************************************************/
int judgeNull_msglist(msg_list_s *q)
{
	if(q->Head == NULL)
	{
		writelog(ZLOG_ERR, "The msglist was null!!!!");
		return -1;
	}
	return 0;
}

/*******************************************************************************
* ������  : del_msglist_node
* ����	  : �ͷ���Ϣ���нڵ�
* ����	  : ����ָ��
* ���	  : ��
* ����	  : 0��ʾִ��������-1����ͷָ��Ϊ��
*******************************************************************************/
int del_msglist_node(msg_list_s *q)
{
	if(q == NULL)
	{
		writelog(ZLOG_ERR, "The msglist was null!!!!");
		return -1;
	}

	msg_node_s *n; 		   				/*������ָ��*/
	if(judgeNull_msglist(q)==-1)    /*�ж϶�ǰ�Ƿ�Ϊ�գ�����Ǿͽ���*/
	{
		writelog(ZLOG_ERR, "The msglist is exist,The msglist was null!");
		return -1;
	}
	n = q->Head;				/* �ڵ�ָ���ͷ */
	q->Head = q->Head->Next;	/* �µĶ�ͷ */
	if(q->Len==1) q->Tail= NULL; /*����ǵ�һ����㣬��βҪָ��Ϊ��*/
	q->Len--;
	free(n);
	return 0;
}






