#include "include.h"

/*******************************************************************************
* 函数名  : init_gwlist
* 功能		: 初始化网关信息队列
* 输入		: 无
* 输出		:
* 返回		: 初始化完的队列管理
*******************************************************************************/
gw_list_s* init_gwlist(void)/*定义指针Q*/
{
	gw_list_s *q;
	if((q =(gw_list_s *)malloc(sizeof(gw_list_s)))==NULL)
	{
		writelog(ZLOG_ERR, "分配存在节点空间出错，此处退出!!");
		return NULL;
	}
	memset(q,0,sizeof(gw_list_s));
	q->Head=NULL;		 /*指定头指针p*/
	q->Tail=NULL;		 /*建立空队列*/
	q->Len=0;			 /*队列成员数量统计*/
	//q->Mtx = PTHREAD_MUTEX_INITIALIZER;
	//q->Cond = PTHREAD_COND_INITIALIZER;

	q->ParkId=0;
	q->GarageId=0;
	
	pthread_mutex_init(&(q->Mtx), NULL);
	pthread_cond_init(&(q->Cond), NULL);
	
	writelog(ZLOG_SYS, "创建网关信息队列成功.");
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
int ins_gwlist(gw_list_s *q, int gwId)
{
	if(q == NULL)
	{
		writelog(ZLOG_ERR, "[ins_gwlist] Invalid argument!");
		return -1;		
	}
	
	if(q->Len >= MAX_GWLIST_LEN)
	{
		writelog(ZLOG_ERR, "超出限定网关最大限定数量");
		return -1;	
	}
	
	gw_node_s *n;
	if((n =(gw_node_s *)malloc(sizeof(gw_node_s)))==NULL)
	{
		writelog(ZLOG_ERR, "插入网关队列内存分配错误");
		return -1;
	}
	
	memset(n,0,sizeof(gw_node_s));
	n->gwID = gwId;
	n->DevErr = GW_COMMU_ERR;			   /* 初始化默认网关设备是没有连接的，所有节点是没有连接的 */
	n->AllSpaceDevErr = DETOR_COMMU_ERR;
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
		writelog(ZLOG_ERR, "超出限定网关最大限定数量");
		return -1;	
	}
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
#endif

