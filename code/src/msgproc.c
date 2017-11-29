#include "include.h"

/*******************************************************************************
* 函数名  : fmtmsg2list
* 功能	  : 格式化消息
* 输入	  : glist,网关队列
		  : cmd,命令
		  : content,内容
		  : len,内容长度
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int fmtmsg2list(msg_list_s* mlist, char cmd, char* content, int len)
{
	if((mlist == NULL)||(cmd == 0))
	{
		writelog(ZLOG_ERR, "[fmtmsg2list] Invalid argument!");
		return -1;
	}
	char buf[MAX_MSG_LEN];

	buf[0]=0x46;
	buf[1]=0x46;
	buf[2]=0x46;
	buf[3]=0x46;

	buf[4]=cmd;

	buf[5]=(len&0x0000ff00)>>8; /* 长度高位 */
	buf[6]=len&0x000000ff; /* 长度地位 */

	memcpy(&buf[7],content,len);

	buf[7+len]=0x64;
	buf[8+len]=0x64;
	buf[9+len]=0x64;
	buf[10+len]=0x64;
	
	return(ins_msglist(mlist,buf,11+len));

}
/*******************************************************************************
* 函数名  : total2msglist
* 功能	  : 车位总数消息
* 输入	  : glist,网关队列
		  : parkid,停车场ID
          : garageid,地库id		  
		  : total,总数
		  : remaintotal,剩余车位数
		  : 在场车位数
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int total2msglist(msg_list_s* mlist,int parkid,int garageid, 
						int total,int remaintotal,int usrinnusetotal,int abntotal)
{
	if((mlist == NULL)||(total == 0))
	{
		writelog(ZLOG_ERR, "[fmttotal2msglist] Invalid argument!");
		return -1;
	}
	char buf[64];
	int len;
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%d;%d;%d;%d;%d;%d;",
		parkid,garageid,total,remaintotal,usrinnusetotal,abntotal);
	len=strlen(buf);
	return(fmtmsg2list(mlist,UPL_TOTAL_CMD,buf,len));
}

/*******************************************************************************
* 函数名  : fmtdetail2msglist
* 功能	  : 插入明细到消息队列
* 输入	  : glist,网关队列
		  : content,内容
		  : len,内容长度
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int detail2msglist(msg_list_s* mlist,int parkid,int garageid, 
						char *detail,int detaillen)
{
	if(mlist == NULL)
	{
		writelog(ZLOG_ERR, "[fmttotal2msglist] Invalid argument!");
		return -1;
	}
	char buf[MAX_MSG_LEN];
	int len;
	memset(buf,0,MAX_MSG_LEN);
	sprintf(buf,"%d;%d;",parkid,garageid);
	len=strlen(buf);
	if(detaillen != 0)
	{
		memcpy(&buf[len],detail,detaillen);
	}
	len=len+detaillen;
	return(fmtmsg2list(mlist,UPL_DETAIL_CMD,buf,len));

}
/*******************************************************************************
* 函数名  : crtmsg
* 功能	  : 建立消息
* 输入	  : glist,网关队列
		  : mlist,消息队列
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int crtmsg(gw_list_s* glist,msg_list_s* mlist,
				int *spacttl,int *spacrmn,int *incnt_incar,int *abn)
{
	if((glist == NULL) || (mlist == NULL))
	{
		writelog(ZLOG_ERR, "[crtmsg] Invalid argument!");
		return -1;
	}
	gw_node_s *n;
	n = glist->Head;
	
	int total=0;
	int remainTotal=0;
	int usrInUseTotal=0;//用户正在使用的车位数，表示车辆正停在车位上的数量
	int abnormal=0;     //异常总数
	
	char buf[MAX_MSG_LEN];
	char tmpbuf[32];
	int i;
	memset(buf,0,MAX_MSG_LEN);
	while(n != NULL)
	{
		if(n->gwID == 0)
		{
			n=n->Next;
			continue;
		}

		for(i=0;i<MAX_GWSPACE_LEN;i++)
		{
			if((n->SpaceInfo[i].SpaceID == 0)
				||(n->SpaceInfo[i].InUse == 0))
			{	
				continue;
			}
			total++;
			if(n->SpaceInfo[i].DevErr != 0)/* 异常总数,异常车位不参与计数 */
			{
				abnormal++;		
			}
			else if(n->SpaceInfo[i].InCnt != 0)/* 有参与计数的车位才能参与统计 */
			{
				if(n->SpaceInfo[i].NowStat == 0)
				{
					remainTotal++;
				}
				else
				{
					usrInUseTotal++;
				}	
			}	
			/* 不管有没有参与计数，只要有这个车位有投用，那么就需要上传明细 */
			/* 不上传传明细的条件是，错误码为00，并且本次状态和上次状态相同 */
			/* 如果有异常车位，那么会一直上传 */
			if((n->SpaceInfo[i].DevErr == 0)
				&&(n->SpaceInfo[i].NowStat == n->SpaceInfo[i].LastStat))
			{
				continue;
			}
			int j;
			memset(tmpbuf,0,sizeof(tmpbuf));/* deverr错误码存储在高4位 */
			j=(n->SpaceInfo[i].DevErr << 4) | n->SpaceInfo[i].NowStat;/* 加上错误码 */
			sprintf(tmpbuf,"%s;%d;",n->SpaceInfo[i].SpaceNO, j);
            //printf("-------------------%s\n",tmpbuf);
			strcat(buf,tmpbuf);
		}
		n=n->Next;
	}
	*spacttl = total;
	*spacrmn = remainTotal;
	*incnt_incar = usrInUseTotal;
	*abn = abnormal;
	
	total2msglist(mlist,glist->ParkId,glist->GarageId,
						total,remainTotal,usrInUseTotal,abnormal);
	detail2msglist(mlist,glist->ParkId,glist->GarageId, 
						buf,strlen(buf));
	return 0;
}

/*******************************************************************************
* 函数名  : crtmsg
* 功能	  : 建立消息
* 输入	  : glist,网关队列
		  : mlist,消息队列
* 输出	  : 无
* 返回	  : 0:正常
			-1:执行失败
*******************************************************************************/
int msg2mng(msg_list_s* mlist,char *mngIP, int mngPort)
{
	if((mlist == NULL) || (mngIP == NULL)|| (mngPort == 0))
	{
		writelog(ZLOG_ERR, "[msg2mng] Invalid argument!");
		return -1;
	}
	msg_node_s *n;
	n = mlist->Head;

	while(n != NULL)
	{
		zmqsend_msg(mngIP, mngPort,n->Msg,n->MsgLen);
		n=n->Next;
		del_msglist_node(mlist);
	}
	return 0;
}




