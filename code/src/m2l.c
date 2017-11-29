#include "include.h"

int gm2lcmd[M2L_CMD_TOTAL]=
{
	REBOOT, 
	TIMING, 
	GET_SPACE_TOTAL, 
	GET_SPACE_DETAIL
};

/*******************************************************************************
* ������  : m2lmsg_anlys
* ����	  : ��������͸����ص���Ϣ����
* ����	  : void *buf : ��Ϣ������
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
int m2lmsg_anlys(char *buf,int len,char *content,int *res,int *cmd)
{
	if((buf==NULL)||(len==0)||(content == NULL))
	{
		writelog(ZLOG_ERR, "[m2lmsg_anlys] Invalid argument!");
		return -1;
	}

	int i,j;
	int clen;

	j=0;
	memcpy(&j,&buf[0],4);		/* ֡ͷ */
	if(j != 0x46464646)
	{
		writelog(ZLOG_ERR,"���ջ�Ӧ֡ͷ����");
		*res=FRAME_HEAD_ERR;
		return -1;
	}
	
	j=0;
	j=buf[4];					/* ������ */
	for(i=0; i<M2L_CMD_TOTAL; i++)
	{
		if(j == gm2lcmd[i])
		{ 
			break;	
		}
	}
	if(i >= M2L_CMD_TOTAL) //����������
	{
		writelog(ZLOG_ERR,"û�и�������:%02X",j);
		*res = FRAME_CMD_ERR;
		return -1;
	}
	*cmd=j;
	
	clen=0;
	clen=(buf[5] << 8) | buf[6];

	if(clen > 0)/* ���Ȳ�Ϊ0 */
	{
		memcpy(content,&buf[7],clen);	
		
	}
	content[clen]='\0';
	j=0;
	memcpy(&j,&buf[7+clen],4);		/* ֡ͷ */
	if(j != 0x64646464)
	{
		writelog(ZLOG_ERR,"���ջ�Ӧ֡β����");
		*res = FRAME_TAIL_ERR;
		return -1;
	}
	return 0;

}

/*******************************************************************************
* ������  : fmtrly_m2lmsg
* ����	  : ��ʽ�����͸��������֡����
* ����	  : void *buf : ��Ϣ������
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
int fmtrly_m2lmsg(char *inbuf,int cmd,int execres,char *outbuf,int *outlen)
{
	if(outbuf == NULL)
	{
		writelog(ZLOG_ERR, "[fmtrly_m2lmsg] Invalid argument!");
		return -1;
	}

	outbuf[0]=0x46;
	outbuf[1]=0x46;
	outbuf[2]=0x46;
	outbuf[3]=0x46;

	outbuf[4]=(char)cmd;
	outbuf[5]=(char)execres;

	int len=0;
	if(inbuf != NULL)
	{
		len=strlen(inbuf);
		outbuf[6]=(char)(len&0x0000ff00)>>8;
		outbuf[7]=(char)len&0x000000ff;		
	}
	else 
	{
		len=0;
		outbuf[6]=0;
		outbuf[7]=0;	
	}

	if((len != 0)&&(inbuf != NULL))
	{
		memcpy(&outbuf[8],inbuf,len);
	}

	outbuf[8+len]=0x64;
	outbuf[9+len]=0x64;
	outbuf[10+len]=0x64;
	outbuf[11+len]=0x64;
	*outlen=len+12;
	return 0;

}

/*******************************************************************************
* ������  : get_spacetotal
* ����	  : ��ȡ��λ����������ͨ��Э��ظ����ݵĸ�ʽ��������
* ����	  : glist,���ض���
* ���	  : res,���Ͱ�������
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int get_spacetotal(gw_list_s* glist,char *res)
{
	if((glist == NULL) || (res == NULL))
	{
		writelog(ZLOG_ERR, "[get_spacetotal] Invalid argument!");
		return -1;
	}
	gw_node_s *n;
	n = glist->Head;
	
	int total=0;
	int remainTotal=0;
	int usrInUseTotal=0;//�û�����ʹ�õĳ�λ������ʾ������ͣ�ڳ�λ�ϵ�����
	int abnormal=0;     //�쳣����
	int i;

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

		}
		n=n->Next;
	}

	sprintf(res,"%d;%d;%d;%d;%d;%d;",gwlist->ParkId, gwlist->GarageId,  
									total,remainTotal,usrInUseTotal,abnormal);
	return 0;
}


/*******************************************************************************
* ������  : get_spacedetail
* ����	  : ��ȡ��λ��ϸ��������ͨ��Э���ʽ�����ظ�֡������
* ����	  : glist,���ض���
* ���	  : res,���Ͱ�������
* ����	  : 0:����
			-1:ִ��ʧ��
*******************************************************************************/
int get_spacedetail(gw_list_s* glist,char *res)
{
	if((glist == NULL) || (res == NULL))
	{
		writelog(ZLOG_ERR, "[get_spacetotal] Invalid argument!");
		return -1;
	}
	gw_node_s *n;
	n = glist->Head;
	
	int i;	
	char tmpbuf[32];
	
	sprintf(res,"%d;%d;",gwlist->ParkId, gwlist->GarageId);
	
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

			int j;
			memset(tmpbuf,0,sizeof(tmpbuf));
			j= n->SpaceInfo[i].DevErr<<4 | n->SpaceInfo[i].NowStat;/* ���ϴ����� */
			sprintf(tmpbuf,"%s;%d;",n->SpaceInfo[i].SpaceNO, j);
			strcat(res,tmpbuf);			
		}
		n=n->Next;
	}
	return 0;
}

/*******************************************************************************
* ������  : exec_m2lmsg
* ����	  : ִ����Ϣ
* ����	  : void *buf : ��Ϣ������
* ���	  : ��
* ����	  : 0:����
			-1:��ʼ���ڴ����
*******************************************************************************/
int exec_m2lmsg(char *content,int cmd,char *res)
{
	if((content==NULL)||(res == NULL))
	{
		writelog(ZLOG_ERR, "[exec_m2lmsg] Invalid argument!");
		return -1;
	}

	switch(cmd)
	{
		case REBOOT:
		{
			writelog(ZLOG_MNG,"[�յ������豸����]");
			//reboot(RB_AUTOBOOT);//����ָ������ִ�У��Ȼظ�ִ�гɹ���֮����ִ��
			break;
		}
		case TIMING:
		{
			writelog(ZLOG_MNG, "[�յ��޸�ϵͳʱ��]: %s",content);
			char buf[64];
			memset(buf, 0, sizeof(buf));
			strcat(buf, "date ");
			strcat(buf, content);
			system(buf);
			sleep(1);
			system("hwclock -w");

			break;	
		}
		case GET_SPACE_TOTAL:
		{
			writelog(ZLOG_MNG, "[�յ���ȡ��λ����ָ��]");
	
			/* �������� */
			pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);
			pthread_mutex_lock(&gwlist->Mtx);
			//pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);

			/* ��ȡ���� */
			get_spacetotal(gwlist,res);

			//��������
			pthread_mutex_unlock(&gwlist->Mtx);
			//pthread_cond_signal(&gwlist->Cond);
			pthread_cleanup_pop(0);
			break;	
		}
		case GET_SPACE_DETAIL:
		{
			writelog(ZLOG_MNG, "[�յ���ȡ��λ��ϸָ��]");	
	
			/* �������� */
			pthread_cleanup_push(thread_free,(void*)&gwlist->Mtx);
			pthread_mutex_lock(&gwlist->Mtx);
			//pthread_cond_wait(&gwlist->Cond, &gwlist->Mtx);

			/* ��ȡ��ϸ�����г�λ��Ϣ */
			get_spacedetail(gwlist,res);

			//��������
			pthread_mutex_unlock(&gwlist->Mtx);
			//pthread_cond_signal(&gwlist->Cond);
			pthread_cleanup_pop(0);
			break;	
		}
		default:
		{
			writelog(ZLOG_ERR, "��ʶ�������:%08x",cmd);
			break;
		}
	}

	return 0;

}

