#include "include.h"

/*******************************************************************************
* ������  : fmtmsg2list
* ����	  : ��ʽ����Ϣ
* ����	  : glist,���ض���
		  : cmd,����
		  : content,����
		  : len,���ݳ���
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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

	buf[5]=(len&0x0000ff00)>>8; /* ���ȸ�λ */
	buf[6]=len&0x000000ff; /* ���ȵ�λ */

	memcpy(&buf[7],content,len);

	buf[7+len]=0x64;
	buf[8+len]=0x64;
	buf[9+len]=0x64;
	buf[10+len]=0x64;
	
	return(ins_msglist(mlist,buf,11+len));

}
/*******************************************************************************
* ������  : total2msglist
* ����	  : ��λ������Ϣ
* ����	  : glist,���ض���
		  : parkid,ͣ����ID
          : garageid,�ؿ�id		  
		  : total,����
		  : remaintotal,ʣ�೵λ��
		  : �ڳ���λ��
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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
* ������  : fmtdetail2msglist
* ����	  : ������ϸ����Ϣ����
* ����	  : glist,���ض���
		  : content,����
		  : len,���ݳ���
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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
* ������  : crtmsg
* ����	  : ������Ϣ
* ����	  : glist,���ض���
		  : mlist,��Ϣ����
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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
	int usrInUseTotal=0;//�û�����ʹ�õĳ�λ������ʾ������ͣ�ڳ�λ�ϵ�����
	int abnormal=0;     //�쳣����
	
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
			if(n->SpaceInfo[i].DevErr != 0)/* �쳣����,�쳣��λ��������� */
			{
				abnormal++;		
			}
			else if(n->SpaceInfo[i].InCnt != 0)/* �в�������ĳ�λ���ܲ���ͳ�� */
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
			/* ������û�в��������ֻҪ�������λ��Ͷ�ã���ô����Ҫ�ϴ���ϸ */
			/* ���ϴ�����ϸ�������ǣ�������Ϊ00�����ұ���״̬���ϴ�״̬��ͬ */
			/* ������쳣��λ����ô��һֱ�ϴ� */
			if((n->SpaceInfo[i].DevErr == 0)
				&&(n->SpaceInfo[i].NowStat == n->SpaceInfo[i].LastStat))
			{
				continue;
			}
			int j;
			memset(tmpbuf,0,sizeof(tmpbuf));/* deverr������洢�ڸ�4λ */
			j=(n->SpaceInfo[i].DevErr << 4) | n->SpaceInfo[i].NowStat;/* ���ϴ����� */
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
* ������  : crtmsg
* ����	  : ������Ϣ
* ����	  : glist,���ض���
		  : mlist,��Ϣ����
* ���	  : ��
* ����	  : 0:����
			-1:ִ��ʧ��
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




