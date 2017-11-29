#include "include.h"

/*******************************************************************************
* 函数名  : init_msglist
* 功能		: 初始化网关信息队列
* 输入		: 无
* 输出		:
* 返回		: 初始化完的队列管理
*******************************************************************************/
msg_list_s* init_msglist(void)/*定义指针Q*/
{
	msg_list_s *q;
	if((q =(msg_list_s *)malloc(sizeof(msg_list_s)))==NULL)
	{
		writelog(ZLOG_ERR, "分配存在节点空间出错，此处退出!!");
		return NULL;
	}
	memset(q,0,sizeof(msg_list_s));
	q->Head=NULL;		 /*指定头指针p*/
	q->Tail=NULL;		 /*建立空队列*/
	q->Len=0;			 /*队列成员数量统计*/
	//q->Mtx = PTHREAD_MUTEX_INITIALIZER;
	//q->Cond = PTHREAD_COND_INITIALIZER;

	pthread_mutex_init(&(q->Mtx), NULL);
	pthread_cond_init(&(q->Cond), NULL);
	
	writelog(ZLOG_SYS, "创建上传消息队列成功.");
	return q;
}

/*******************************************************************************
* 函数名  : ins_gwlist
* 功能	  : 插入队列节点
* 输入	  : *q:队列指针
			*n:单元节点
* 输出	  : 无
* 返回	  : 0:正常
			-1:错误
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
		writelog(ZLOG_ERR, "超出限定消息队列最大限定数量");
		return -1;	
	}
	
	msg_node_s *n;
	if((n =(msg_node_s *)malloc(sizeof(msg_node_s)))==NULL)
	{
		writelog(ZLOG_ERR, "插入消息队列内存分配错误");
		return -1;
	}
	
	memset(n,0,sizeof(msg_node_s));
	n->MsgLen = msglen;
	memcpy(n->Msg, msg, msglen);
	n->Next = NULL; 		   /* n的下一个为空 */
	
	if(q->Tail != NULL)
	{
		q->Tail->Next = n;
	}
	q->Tail = n;		   		/* 队尾节点指向n */
	if(q->Head == NULL)
	{
		q->Head = n;
	}
	q->Len = q->Len + 1;		/* 长度加一 */
	return 0;
}


/*******************************************************************************
* 函数名  : judgeNull_SqlopQueue
* 功能	  : 判断数据库操作队列中的是否为空
* 输入	  : stru_sqlopq *q:数据库操作链队列

* 输出	  :
* 返回	  : 0:不为空
			-1:为空状态
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
* 函数名  : del_msglist_node
* 功能	  : 释放消息队列节点
* 输入	  : 队列指针
* 输出	  : 无
* 返回	  : 0表示执行正常，-1队列头指针为空
*******************************************************************************/
int del_msglist_node(msg_list_s *q)
{
	if(q == NULL)
	{
		writelog(ZLOG_ERR, "The msglist was null!!!!");
		return -1;
	}

	msg_node_s *n; 		   				/*定义结点指针*/
	if(judgeNull_msglist(q)==-1)    /*判断对前是否为空，如果是就结束*/
	{
		writelog(ZLOG_ERR, "The msglist is exist,The msglist was null!");
		return -1;
	}
	n = q->Head;				/* 节点指向队头 */
	q->Head = q->Head->Next;	/* 新的对头 */
	if(q->Len==1) q->Tail= NULL; /*如果是第一个结点，队尾要指向为空*/
	q->Len--;
	free(n);
	return 0;
}






